winter contains various components specified by codenames - to contribute it's important to put 
it in the right place.

| codename  | function                                                                                                                              |
|-----------|---------------------------------------------------------------------------------------------------------------------------------------|
| emley     | Versioning. Contains the Kachemak versioning system.                                                                                  |
| moss      | Utility functions used in other modules. JSON and zip dependencies go here.                                                           |
| coldfield | GDExtension binding code and associated helpers.                                                                                      |
| sheffield | "Big" downloading. Emley uses this for downloading the requisite files needed, so this is where for example a torrent interface'd go. |
| palace    | "main" module, all other modules are used here. Code flow begins here.                                                                |

There's a .clang-tidy file in the root of the repo for consistent formatting.