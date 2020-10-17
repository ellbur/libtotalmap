
# Purpose

`libtotalmap` is a collection of C++ utilities that make it easy to implement new keyboard layouts on Linux using the raw kernel event-handling mechanism---that is, `/dev/input`. It also serves as a reference example if you are looking to write your own software using `/dev/input`.

Writing keyboard layouts in regular code opens the door to doing far more than remapping some keys to other keys. For example, the following are all theoretically possible with `libtotalmap`:

1. Global modal editing, similar to Vim, but universal across your system.
2. Reordering keypresses: every time you type <kbd>a</kbd>, <kbd>b</kbd>, it comes out <kbd>b</kbd>, <kbd>a</kbd>.
3. Using combinations of ordinary letter keys as hotkeys. For example, you could make it so pressing <kbd>f</kbd> and <kbd>j</kbd> at the same time sends <kbd>Esc</kbd>.

The reasons for using `/dev/input`---as opposed to XKB, xmodmap, or analogous Wayland utilities---are the following:

1. **Flexibility**: with `/dev/input`, your keyboard layout is just code. If you can code it, it can do it. XKB is comparatively limited.

2. **Control**: if you use XKB, you will find that some software doesn't play nice with nonstandard layouts. Typical examples are VMs, remote desktop clients, window managers, or anything written in Java. With `/dev/input`, you see typed keys before anyone else and get the first shot at remapping them. The result is that your layout will behave the same with all software.

3. **Portability** (though see below for limitations): `/dev/input` works on any Linux system, including ChromeOS (in developer mode). It does not care what window manager you use, or whether you use X or Wayland.

The disadvantages of using `/dev/input` include the following:

1. **Security**: you cannot use `/dev/input` if you do not have write access to `/dev/uinput`, which usually means superuser access. On ChromeOS, this means developer mode. This could also be considered a security hole since your keyboard layout will have access to everything you type (including passwords) and therefore must be trusted code.

2. **Portability**: `/dev/input` only works on Linux.

3. **Robustness**: "if you can code it, it can do it" applies to bugs in your code as well. For example, it's easy to accidentally let keys get stuck in the "down" position.

4. **Tooling**: common keyboard-layout switchers won't know what to do with your layout. You can of course make your own, since it's just code, but it will require more work.

5. **Unicode Support**: because `/dev/input` operates directly on keyboard devices, it only understands keyboard keys, not characters. For example, there is no difference between lowercase and uppercase letters: `'A'` is just `Shift + A`. The consequence is that there is no canonical way to include non-Latin characters in your layout. To do that, you will have to resort to XKB, an input method, or similar tools. The two can of course be used together: you can use `/dev/input` to do the things XKB can't, then pass the codes along to XKB for Unicode support.

# Caveat

This code is not thoroughly tested and still has bugs. However, it works surprisingly well considering.

This code can see everything you type, and, if exploited by an attacker, could be used to steal your passwords, credit card number, or other personal info. Do not use this code where security is critical, and never allow untrusted code to access `/dev/input`.

# Dependencies

`libtotalmap` uses standard C++17, various Linux headers, and:

* [Boost Range](https://www.boost.org/doc/libs/1_72_0/libs/range/doc/html/index.html), which can be installed from the package manager on most Linux distros; and
* [`nlohmann/json`](https://github.com/nlohmann/json). `nlohmann/json` is included under `third-party/` for convenience, so you do not need to obtain it.

Additionally, the examples depend on:

* [Boost.ProgramOptions](https://theboostcpplibraries.com/boost.program_options), which can be installed from the package manager on most Linux distros.

# Building

There are simple GNU Makefiles that merely invoke `$(CXX)`.

To build a shared library, run:

    ./build-shared-library.sh

To build example programs, run:

    ./build-all-examples.sh

To use `libtotalmap` in your own software, you could link with the shared library, or, since the code is small, just link all files under `src/` into your own project.

# Configuring Permissions

You will need write permissions to `/dev/uinput` to generate key codes, and to `/dev/input/<your-actual-keyboard>` to prevent the original key codes from getting through.

Depending on your system, `/dev/input` and `/dev/uinput` might belong to groups `input`, `uinput`, or each of those, respectively. They might or might not be group-writable.

If those groups do not yet exist on your system, you may create them with:

    sudo groupadd input        
    sudo groupadd uinput        

To add yourself to the necessary groups, use, as appropriate:

    sudo usermode -a -G input yourusername
    sudo usermode -a -G uinput yourusername

To change device permissions in a way that persists across boots, create a file `/etc/udev/rules.d/99-uinput.rules` and add the following, changing the group to `uinput` if appropriate:
   
	KERNEL=="uinput", MODE="0660", GROUP="input", OPTIONS+="static_node=uinput"
	SUBSYSTEM=="misc", KERNEL=="uinput", MODE="0660", GROUP="input"

# Using on ChromeOS

You must enable developer mode.

Theoretically, you can use access `/dev/input` directly from a ChromeOS developer shell. However, in practice, you usually won't have the execution environment you want in the base system (e.g., `libstdc++`). Although `libtotalmap` could be rewritten to target the ChromeOS environment, that is not something I plan on doing anytime soon. So, it will usually be necessary to install [Crouton](https://github.com/dnschneid/crouton) and run your keyboard layout from within a chroot. 

In crouton, you have access to `/dev/` just as if you were running in the base system (since it's just a chroot). The issue is setting permissions in a way that works from the chroot. This requires mapping the GIDs from the base system to GIDs in the chroot. 

dnschneid has included some GID mappings in [`enter-chroot`](https://github.com/dnschneid/crouton/blob/master/host-bin/enter-chroot). Unfortunately, the `input` group is not among them. So, you can add it by editing `/usr/local/bin/enter-chroot`. Find this code:

    # Fix group numbers for critical groups to match Chromium OS. This is necessary
    # so that users have access to shared hardware, such as video and audio.
    gfile="$CHROOT/etc/group"
    if [ -f "$gfile" ]; then
        for group in audio:hwaudio cras:audio cdrom chronos-access:crouton \
                     devbroker-access dialout disk floppy i2c input lp serial \
                     tape tty usb:plugdev uucp video wayland; do

and add `input` to the list of groups, so that it becomes:

    # Fix group numbers for critical groups to match Chromium OS. This is necessary
    # so that users have access to shared hardware, such as video and audio.
    gfile="$CHROOT/etc/group"
    if [ -f "$gfile" ]; then
        for group in audio:hwaudio cras:audio cdrom chronos-access:crouton \
                     devbroker-access dialout disk floppy i2c input lp serial \
                     tape tty usb:plugdev uucp video wayland input; do

You may also need to add your user to the `input` group from within the chroot.

You cannot use `/dev/input` from crostini because you cannot access `/dev/` from crostini, for obvious security reasons.

# API

The API provides two things:

1. A simple interface to `/dev/input`, and
2. Utilities for translating keypresses to simulate a desired layout.

## Simple interface to `/dev/input`

The basic idea is you write a function that:

1. Takes as input an input keypress, and
2. Returns as output a list of any output keypresses.

In more detail, you write a `DevInputHandler`:

    enum DevInputValue {
      pressed = 1,
      released = 0,
      repeated = 2,
    };

    struct DevInputEvent {
      int code;
      DevInputValue value;
    };

    typedef std::function<list<DevInputEvent>(DevInputEvent const&)> DevInputHandler;

For definitions of keycodes, see the [Linux header](https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h).

For example, here is a `DevInputHandler` that swallows all keys and produces no output:

    auto myHandler = [](DevInputEvent const& next) {
      return { };
    };

Here is one that simply translates one letter to another:

    auto myHandler = [](DevInputEvent const& next) {
      if (next.code == 30) {
        next.code = 31;
      }
      return { next };
    };

Remember, you get separate function calls for presses and releases, and you need to emit separate `DevInputEvent`s for presses and releases. 

Here is one that forces the letter <kbd>a</kbd> to allways be shifted, by inserting a shift-press before it is pressed and a shift-release after it is released:

    auto myHandler = [](DevInputEvent const& next) {
      if (next.value == pressed) {
        if (next.code == 30) {
          return {
            { 42, pressed },
            next
          };
        }
      }
      else {
        if (next.code == 30) {
          return {
            next,
            { 42, released }
          };
        }
      }
    };

However, the above example doesn't handle interactions between the fake shift and the real shift very well. That's why you will in practice need to write more elaborate code or use the utilities described below. 

In general, you *should not* emit key repeats---just presses and releases. Userland software will add in the repeats for you. So, you should silently swallow events with `value == repeat`.  

## Utilities for translating keypresses

In practice, there are a lot of funny edge cases involved in mapping key events from one layout to another. So, `libtotalmap` provides from APIs that help you handle those cases. You can use them yourself or use them as a guide.

### PhysicalLayout

The first abstraction is the physical layout---that is, how keys are arranged in space on your keyboard.

Theoretically, there's no need for code using `/dev/input` to know anything about the physical layout---it's just about mapping codes to other codes. The only reason for specifying the physical layout is to make the virtual layout easier to specify, because you can write it in geometric order rather than as a list of codes. But if you don't care about writing things in geometric order, you can skip over `PhysicalLayout`---it doesn't actually add any functionality.

If you are using a U.S. ANSI keyboard, you can use `ANSIWithWin`, defined in `include/standard-physical-keyboards.hpp`.

### KeyboardLayout

The next abstraction is what you want the keycodes to be when you press a key. For that, you define a `KeyboardLayout`, as specified in `include/keyboard-model.hpp`.

For an example of how to populate this struct, see `examples/complicated-example-layout/my-keyboard-layout.cpp`:

    const string my_top_row = "17531902468`";
    const string my_q_row = ";,.pyfgcrl~@";
    const string my_a_row = "aoeuidhtns-";
    const string my_z_row = "'qjkxbmwvz";

    const string my_top_row_shift = "17531902468`";
    const string my_q_row_shift = ":<>PYFGCRL?^";
    const string my_a_row_shift = "AOEUIDHTNS@";
    const string my_z_row_shift = "\"QJKXBMWVZ";

    const string my_top_row_altgr = "";
    const string my_q_row_altgr = " {}% \\*][|  ";
    const string my_a_row_altgr = "   = &)(/_$";
    const string my_z_row_altgr = "     !+#  ";

    auto addToRow = [&](vector<LayoutKey> &row, string const& chars) {
      for (size_t i=0; i<chars.size(); i++) {
        char c = chars[i];
        if (c == ' ') {
          row.push_back(NullLayoutKey { });
        }
        else {
          row.push_back(CharLayoutKey { c });
        }
      }
    };

    addToRow(layout.k1Row, my_top_row);
    addToRow(layout.qRow, my_q_row);
    addToRow(layout.aRow, my_a_row);
    addToRow(layout.zRow, my_z_row);

    addToRow(layout.k1RowShift, my_top_row_shift);
    addToRow(layout.qRowShift, my_q_row_shift);
    addToRow(layout.aRowShift, my_a_row_shift);
    addToRow(layout.zRowShift, my_z_row_shift);

    addToRow(layout.k1RowAltGr, my_top_row_altgr);
    addToRow(layout.qRowAltGr, my_q_row_altgr);
    addToRow(layout.aRowAltGr, my_a_row_altgr);
    addToRow(layout.zRowAltGr, my_z_row_altgr);

    layout.tilde = CodeLayoutKey { 125 };
    layout.tildeShift = CodeLayoutKey { 125 };
    layout.tildeAltGr = CodeLayoutKey { 125 };

    layout.leftWin = CodeLayoutKey { 100 };

    layout.qRowAltGr[0] = CodeLayoutKey { 1 };

## FullMappingSet

Once you have a `PhysicalLayout` and a `KeyboardLayout`, you can combine them together into a `FullMappingSet`:

    MyKeyboardLayout layout;
    MyPhysicalKeyboard phys;

    FullMappingSet full = joinMappings(phys.layout, layout.layout);

A `FullMappingSet` is just an `std::map` translating from a `TypedKey` to a `PhysRevKey`---that is, from the key you press to the key that should be emitted, including which modifier keys (<kbd>Shift</kbd> and <kbd>AltGr</kbd>) should be down at the same time.

You can also build the `FullMappingSet` in one go if you like without going through `PhysicalLayout` and `KeyboardLayout`, since it's just an `std::map` from codes to codes. For example, here is a `FullMappingSet` that turns lower-case `a` into lower-case `s`:

    FullMappingSet full = {
      {
        { TypedKey { 30, false, false }, PhysRevKey { 31, false } },
      }
    };

## RemappingHandler

Finally, you can take your `FullMappingSet` and pass it to `RemappingHandler`, also telling `RemappingHandler` which keys should be considered modifiers:

    const int leftShift = 42;
    const int rightShift = 54;
    const int leftAlt = 56;
    const int rightAlt = 100;
    const int capsLock = 58;
    const int leftWin = 125;
    const int leftControl = 29;
    const int rightControl = 97;

    RemappingHander remapping(full,
      { leftShift, rightShift }, // Shift keys
      { rightAlt, capsLock, leftWin }, // AltGr keys
      { leftAlt, leftControl, rightControl } // Other modifiers
    );

(The reason `RemappingHandler` needs to handle modifiers separately is that these keys can be typed simultaneously, whereas letter keys can only be typed one at a time.)

`RemappingHandler` defines the `handle` method to translate input keys to output keys:

    list<DevInputEvent> handle(DevInputEvent const&);

As you can see, this has the same signature as `DevInputHandler`, so it can finally be used to remap keycodes:

    runDevInputLoop(keyboardFilePath, "simple_example_layout", trace, [&](DevInputEvent const& ev) {
      return remapping.handle(ev);
    });

You can also see that `handle()`, being a function, is in a sense composable, so you can compose multiple remappings:

    runDevInputLoop(keyboardFilePath, "compose_example", trace, [&](DevInputEvent const& ev) {
      auto r1 mapping1.handle(ev);

      list<DevInputEvent> r2;
      for (auto ev2 : r1) {
        auto rr2 = mapping2.handle(ev2);
        for (auto ev3 : rr2) {
          r2.push_back(ev3);
        }
      }

      return r2;
    });

I say "in a sense" because `handle()` deals with various funny edge cases that may or may not degrade in quality as you compose layouts together.

## Movement

`libtotalmap`, being just code, can do far more than just map keys to other keys. If you can code it, it can do it.

So far, I've only used `libtotalmap` to add one more elaborate feature: movement keys that can be accessed on the letter keys---without moving your hand over to the arrow keys. This is a huge time saver, and now I can barely live without it. 

The code for this is defined in `include/basic-movement-loop.hpp` and `src/basic-movement-loop.cpp`. `BasicMovementLoop` exposes a `handle()` function that also conforms to `DevInputHandler`. So, you can combine movement keys with another keyboard layout:

    runDevInputLoop(keyboardFilePath, "send_b", trace, [&](DevInputEvent const& ev) {
      auto r1 = movement.handle(ev);

      list<DevInputEvent> r2;
      for (auto ev2 : r1) {
        auto rr2 = remapping.handle(ev2);
        for (auto ev3 : rr2) {
          r2.push_back(ev3);
        }
      }

      return r2;
    });

Then, you can access arrow keys by holding down <kbd>Tab</kbd> and pressing:

* <kbd>J</kbd> - Left
* <kbd>K</kbd> - Down
* <kbd>L</kbd> - Right
* <kbd>I</kbd> - Up
* <kbd>U</kbd> - Page Up
* <kbd>M</kbd> - Page Down
* <kbd>H</kbd> - Home
* <kbd>;</kbd> - End
* <kbd>N</kbd> - Ctrl + Left (back one word in most software)
* <kbd>,</kbd> - Ctrl + Right (forward one word in most software)

`BasicMovementLoop` is hard-coded to the above mappings; it's not configurable. But, the code is short, and you can easily customize `src/basic-movement-loop.cpp` to your preferences.

# Running at login

On systems using `systemd`, you can put a service in `~/.config/systemd/user/keymapping.service`:

    [Unit]
    Description=Runs the key mapper

    [Service]
    Type=simple
    ExecStart=/path/to/your/remapping/program

    [Install]
    WantedBy=default.target

and start it with

    systemctl --user start keymapping.service

and enable it to run automatically with

    systemctl --user enable keymapping.service

# License

Permission granted to use under the terms of the [WTFPL](http://www.wtfpl.net/).

