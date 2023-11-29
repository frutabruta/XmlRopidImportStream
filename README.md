# XmlRopidImportStream
Qt library to import Ropid XML to SQLite database (QXmlStreamReader version)

## Changelog
- 20231129
    - data.sqlite
        - fix of accidental removal of APC tables (apc_dpp and apc_opv)
- 20231127
    - row id removed from table z (conflicted with row id)
- 20231025
    - xmlimportjr: unknown tag does not stop import
- 20230921
    - XmlImportJr::vlozT new columns added to import
    - XmlImportJr removal of definition of vacuum from header file
- 20230818
    - new classes to import APC files
    - rename of timetables import classes
    - new tables in data.sqlite
