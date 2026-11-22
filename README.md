# Recycled-Mesh-Texter
Generic code for running meshtastic or meshcore messaging on custom/repurposed hardware using microcontrollers and simple displays

## Layout

| Folder | Contents |
| --- | --- |
| `core/` | Portable code: interfaces (`ITextDisplay`, `IKeySource`, `IMessagingBackend`, `IStorage`), message data types, `TextViewport` (wrap, paging, marquee), `FontSystem`, `KeyMapper`, `MessageStore` |
| `backends/` | Protocol adapters. `fake/` is an offline stand-in for testing |
| `sim/` | Terminal display and keyboard drivers plus the simulator entry point |
| `ui/` | Device UI |
| `tests/` | Host-side checks for `core/` |

## Building on a host (Codespaces, Linux, macOS)

```
make sim     # run the simulator with an Ace-like 16x1 display
make test    # run the core tests
```

Other display shapes: `./build/sim --cols 20 --rows 4 --ascii`
