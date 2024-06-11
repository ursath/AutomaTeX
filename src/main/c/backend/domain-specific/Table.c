#include "Table.h"


// EL CPY lo hace el usuario
Value getValue(char * identifier, ValueType type){
    Entry result;
    //result = hs_get( identifier)
    if ( result==NO_RESULT || type != result.type )
        return NO_RESULT;
    return result.value;
}

// todo 
boolean exists(char * identifier ) {
    //return hs_exist
    return false; 
}

boolean exists(char * identifier, ValueType type ) {
    Entry entry; // = hs_get(identifier);
    return entry!=NULL && entry.type==type;
}

boolean insert(char * identifier,  ValueType type, Value value ){
    Entry * entry = malloc(sizeof(Entry));
    entry->type = type;
    entry->value = value;
    
    if( exists(identifier))
        return false; 
    // hs_put
    return true;
}

/*
¿Puedo usar librerías externas?
Sí, en cuyo caso primero se deberá solicitar aprobación vía correo al NS-QRF indicando la necesidad arquitectural y/o tecnológica de la misma. 
Luego de aprobada, se deberá referenciar en el README y en el informe final.
*/