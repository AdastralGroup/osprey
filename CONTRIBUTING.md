winter contains various components specified by codenames - to contribute it's important to put 
it in the right place.

| codename  | function                                                                                                                              |
|-----------|---------------------------------------------------------------------------------------------------------------------------------------|
| emley     | Versioning. Contains the Kachemak versioning system.                                                                                  |
| fremont   | Utility functions used in other modules. JSON and zip dependencies go here.                                                           |
| coldfield | GDExtension binding code, boilerplate and associated helpers.                                                                         |
| sheffield | "Big" downloading. Emley uses this for downloading the requisite files needed, so this is where for example a torrent interface'd go. |
| palace    | "main" module, handles southbank parsing, manages high level functions.                                                               |
| sutton    | Class that connects the GDExtension side and palace.                                                                                  |
| beacon    | Driver code to test all the non-Godot parts / Somewhat effective TUI                                                                  |


There's a .clang-tidy file in the root of the repo for consistent formatting.