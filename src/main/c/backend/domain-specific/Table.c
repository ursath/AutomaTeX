#include "Table.h"
#include "khash.h"


typedef struct {
	Value value;        //TODO: Node q contiene first, tail=value
    ValueType type;
} Entry;

static Logger * _logger = NULL;

KHASH_MAP_INIT_STR(myhash, Entry)

khash_t(myhash) * hashTable;


void initializeTable(void){
   hashTable = kh_init(myhash);
   _logger = createLogger("Table");
}

// EL CPY lo hace el usuario
EntryResult getValue(char * identifier, ValueType type){
    logWarning(_logger, "Getting value with identifier: %s", identifier);
    EntryResult result = { .found=false} ;
    khiter_t k = kh_get(myhash, hashTable, identifier);
    if ( k==kh_end(hashTable) )
        return result;
    Entry entry = kh_value(hashTable,k);
    if ( type == entry.type ) {
        result.found=true, 
        result.value=entry.value;
    }
    return result;
}

boolean exists(char * identifier ) {
    khiter_t k = kh_get(myhash, hashTable, identifier);
    if ( k==kh_end(hashTable) )
        return false;
    return true; 
}

//
//boolean exists(char * identifier, ValueType type ) {
//    Entry entry; // = hs_get(identifier);
//    return entry!=NULL && entry.type==type;
//}

boolean insert(char * identifier,  ValueType type, Value value ){
    int ret;
    logWarning(_logger, "Arrived to insert()");
    logWarning(_logger, "Identifier: %s", identifier);
    if ( hashTable != NULL)
        logWarning(_logger, "Hash table exists");
    khiter_t k = kh_put(myhash, hashTable, identifier, &ret);
    if ( ret <= 0)
        return false;
    logCritical(_logger, "Put key");
    Entry entry = {
        .type = type,
        .value = value
    };

    kh_value(hashTable,k) = entry;
    return true;
}

/*
¿Puedo usar librerías externas?
Sí, en cuyo caso primero se deberá solicitar aprobación vía correo al NS-QRF indicando la necesidad arquitectural y/o tecnológica de la misma. 
Luego de aprobada, se deberá referenciar en el README y en el informe final.
*/
