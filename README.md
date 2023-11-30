# XmlRopidImportStream
Qt library to import Ropid XML to SQLite database (QXmlStreamReader version)

## Changelog
- 20231130_2
    - new functions 
        - XmlImportJr::truncateApc()
        - XmlImportJr::truncateTimetables()
        - XmlImportJr::truncateTasks()
    - data.sqlite
        - test data from ukoly_zaznam removed
- 20231130    
    - new imported columns:
        - d.ico (now integer), d.tel, d.teli        
        - z.n2, z.n3, z.n4, z.n5, z.n6, z.n7, z.n8, z.ois2, z.cco, z.nco
        - z.sy (value fix), z.ve, z.xD
        - s.icls
    - data.sqlite
        - tables modification

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
