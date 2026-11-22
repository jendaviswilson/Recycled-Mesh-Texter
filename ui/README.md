# ui/

The device UI. It talks only to the core interfaces (`ITextDisplay`, `KeyEvent` from `KeyMapper`,
`TextViewport`, `MessageStore`, `IMessagingBackend`), so the same UI runs in the terminal simulator
and on hardware.

`sim/main.cpp` contains a placeholder `DemoUi` until this folder has a real UI.
