winter contains various components specified by codenames - to contribute it's important to put 
it in the right place. Important modules are given special names, secondary modules are given pragmatic names.

| codename | function                                                                            |
|----------|-------------------------------------------------------------------------------------|
| emley    | Versioning. Contains the Kachemak versioning system.                                |
| palace   | "main" module, handles southbank parsing, manages high level functions.             |
| sys      | Contains filesystem related functions, such as SDK detection and zip functionality. |
| net      | Contains a wrapper around cURL currently, and some helpers.                         |
| binding  | GDExtension binding code and associated helpers.                                    |
| torrent  | Torrent interface module.                                                           |
| headless | Headless mode, to assist with debugging.                                            |
| shared   | Shared code between functions - contains events system.                             |


There's a .clang-format file in the root of the repo for consistent formatting.