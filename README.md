# XmlRopidImportStream
Qt library to import Ropid XML to SQLite database (QXmlStreamReader version)

## Changelog
- 20241230
    - XML version 1.37 and 1.38 compatiblity changes
    - data.sqlite
        - table hlavicka
            - new value exp
            - new value ver (starting from version 1.37)
            - new value filepath
        - table z
            - new value kmen (starting from version 1.37)
            - new value neprep (starting from version 1.37)
        - table s
            - new value skol (starting from version 1.37) 
            - new value lc (starting from version ???) 
            - new value obl (starting from version 1.38) 
            - new value jk (starting from version ???) 
            - new value sp1 (starting from version ???) 
            - new value spN (starting from version ???) 
            - new value doh (starting from version ???) 
            - new value pos (starting from version ???) 
            - new value skol (starting from version ???) 
        - table po (added as a draft for future XML release (1.39?))
            - new value thls
            - new value tpan
            - new value kan
            - new value akce
            - new value zkr3 (starting from version ???)
            - new value vjr (starting from version ???)
            - new value u (starting from version ???)
            - new value z (starting from version ???)
            - new value u2 (starting from version ???)
            - new value z2 (starting from version ???)
            - new value usm (starting from version ???)
            - new value mind (starting from version ???)

        - table o
            - new value vyb (starting from version 1.37) 
        - table x
            - new value neozn (starting from version 1.38) 
            - new value zast (starting from version 1.38) 
        - new table obl
    - XmlImportJr
        - import obl
        - added obl to truncateTimetables
        - import new table values (same as added in data.sqlite)

- 20240726
    - fix of "QSqlDatabasePrivate::database: requested database does not belong to the calling thread." in RopidXmlTools 
    - this->mojeDatabaze = QSqlDatabase::addDatabase("QSQLITE"); moved from SqLiteZaklad::pripoj() to SqLiteZaklad::otevriDB()
- 20240617
    - SqLiteZaklad
        - SqLiteZaklad::vytvorCasHodinyMinuty
            - if value from DB is empty string, returns empty string (used to return 00:00)
- 20240514
    - XmlImportJr::vlozS
        - added import of columms: sm, np
- 20240306
    - data.sqlite
        - modified primary key to allow more lines with same license number (needed for RopidXmlTools)
- 20231206
    - DB file update
- 20231204
    - XmlImportJr::vlozG
        - new columns: zvd,pd
    - XmlImportJr::vlozX
        - new columns: po,bp,zpl,kpl,s,dm, xS, xTra, xBus, xTro, xLan
        - column reorder: xA, na, s1, s2
        - column fix: xD

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
