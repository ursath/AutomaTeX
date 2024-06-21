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
    logInformation(_logger, "Getting value with identifier: %s...", identifier);
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


boolean insert(char * identifier,  ValueType type, Value value ){
    int ret;
    logWarning(_logger, "Inserting identifier: %s...", identifier);

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
