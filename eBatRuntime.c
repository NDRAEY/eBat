#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eBat.h"
#include "eBatRuntime.h"

void bat_runtime_echo(BAT_T* bat,BAT_GROUP_T* group){
     if (bat->Echo == 1){
        for (int i = 0; i < group->Size; i++){
            BAT_TOKEN_T *Line = (BAT_TOKEN_T *) group->Tokens[i];
            if (Line->type == BAT_TOKEN_TYPE_VARIABLE) {
                printf("%c%s%c ", '%', Line->value,'%');
            } else if (Line->type == BAT_TOKEN_TYPE_STRING) {
                printf("\"%s\" ", Line->value);
            } else {
                printf("%s ", Line->value);
            }

        }
        printf("\n");
    }
    if (bat->Debug == 1){
        for (int y = 0; y < group->Size; y++){
            BAT_TOKEN_T* xtok = (BAT_TOKEN_T*) group->Tokens[y];
            bat_debug(" |--- [%d | %d] Tok: [%d] %s | Val: %s \n", y+1, group->Size,  xtok->type, bat_debug_type(xtok->type), xtok->value);
        }
    }
}

int bat_runtime_equal(int Line, BAT_TOKEN_T* Data1, BAT_TOKEN_T* Eq, BAT_TOKEN_T* Data2) {
    if (Data1->type != Data2->type || (Data1->type != BAT_TOKEN_TYPE_VARIABLE && Data1->type != BAT_TOKEN_TYPE_NUMBER)) {
        bat_fatalerror(Line, "You cannot compare different types of data. Only NUMBERS and VARIABLES are allowed to be compared.");
        return 0;
    }
    if (Data1->type == BAT_TOKEN_TYPE_VARIABLE) {
        eBatCheckModule(Line, EBAT_CONFIG_SYSTEM_SET, "System.Set");
        eBatCheckVariable(Line, Data1->value, D1);
        eBatCheckVariable(Line, Data2->value, D2);
        if (D1 == NULL || D2 == NULL){
            if (D1 != NULL) free((void*) D1);
            if (D2 != NULL) free((void*) D2);
            return 0;
        }

        int ret = (strcmp(D1, D2) == 0?1:0);
        return ret;
    } else if (Data1->type == BAT_TOKEN_TYPE_NUMBER){
        int a = bat_strtol(Data1->value);
        int b = bat_strtol(Data2->value);
        switch (Eq->type) {
            case BAT_TOKEN_TYPE_EQUAL:{
                return (a == b?1:0);
            }
            case BAT_TOKEN_TYPE_NOT_EQUAL:{
                return (a != b?1:0);
            }
            case BAT_TOKEN_TYPE_GREATER:{
                return (a > b?1:0);
            }
            case BAT_TOKEN_TYPE_LESS:{
                return (a < b?1:0);
            }
            case BAT_TOKEN_TYPE_LESS_EQUAL:{
                return (a <= b?1:0);
            }
            case BAT_TOKEN_TYPE_GREATER_EQUAL:{
                return (a >= b?1:0);
            }
            default:
                return 0;
        }
    }
    return 0;
}

int bat_runtime_eval(BAT_T* bat,BAT_GROUP_T* group, int line, int offset) {
    if (group->Size <= 0) {
        return 0;
    }

    BAT_TOKEN_T *MainTok = (BAT_TOKEN_T *) group->Tokens[offset + 0];

    if (MainTok->type == BAT_TOKEN_TYPE_UNKNOWN) {
        return 0;
    }

    if (MainTok->type == BAT_TOKEN_TYPE_EXIT) {
        EBAT_INVALIDARGC(line, group->Size - offset, 2);
        BAT_TOKEN_T *Data1 = (BAT_TOKEN_T *) group->Tokens[offset + 1];
        return (-1 * bat_strtol(Data1->value));
    }

    if (MainTok->type == BAT_TOKEN_TYPE_ECHO) {
        EBAT_INVALIDARGC(line, group->Size - offset, 2);
        BAT_TOKEN_T *TextTok = (BAT_TOKEN_T *) group->Tokens[offset + 1];
        if (TextTok->type == BAT_TOKEN_TYPE_DEBUG){
            bat->Debug = !(bat->Debug);
        } else if (TextTok->type == BAT_TOKEN_TYPE_TRUE){
            bat->Echo = 1;
        } else if (TextTok->type == BAT_TOKEN_TYPE_FALSE){
            bat->Echo = 0;
        } else if (TextTok->type == BAT_TOKEN_TYPE_STRING){
            bat_runtime_system_echo(TextTok->value);
        } else if (TextTok->type == BAT_TOKEN_TYPE_VARIABLE){
            eBatCheckModule(line, EBAT_CONFIG_SYSTEM_SET, "System.Set");
            eBatCheckVariable(line, TextTok->value, Text);
            bat_runtime_system_echo(Text);
        } else if (TextTok->type == BAT_TOKEN_TYPE_NUMBER){
            bat_runtime_system_echo(TextTok->value);
        }
        return 0;
    }

    if (MainTok->type == BAT_TOKEN_TYPE_IF) {
        EBAT_INVALIDMINARGC(line, group->Size - offset, 3);

        int isNOT = 0;

        BAT_TOKEN_T *DataNOT = (BAT_TOKEN_T *) group->Tokens[offset + 1];
        if (DataNOT->type == BAT_TOKEN_TYPE_NOT){
            isNOT = 1;
        }


        BAT_TOKEN_T *Data1 = (BAT_TOKEN_T *) group->Tokens[offset + isNOT + 1];
        BAT_TOKEN_T *Data2 = (BAT_TOKEN_T *) group->Tokens[offset + isNOT + 2];
        BAT_TOKEN_T *Data3 = (BAT_TOKEN_T *) group->Tokens[offset + isNOT + 3];

        if (Data1->type == BAT_TOKEN_TYPE_EXIST) {
            eBatCheckModule(line, EBAT_CONFIG_FILEIO_EXIST, "FileIO.Exits");
            eBatCheckMixingData(line, Data2->type, BAT_TOKEN_TYPE_VARIABLE);
            eBatCheckModule(line, EBAT_CONFIG_SYSTEM_SET, "System.Set");
            eBatCheckVariable(line, Data2->value, Path);

            int breq = bat_runtime_fileio_exist(Path);
            if ((breq == 1 && isNOT == 0) ||
                (breq != 1 && isNOT == 1)){
                bat_runtime_eval(bat, group, line, offset + isNOT + 4);
            }

            bat_debug("breq: %d | xret\n", breq);

        } else if (Data1->type == BAT_TOKEN_TYPE_NUMBER) {
            EBAT_INVALIDMINARGC(line, group->Size - offset, offset + 4);

            int breq = bat_runtime_equal(line, Data1, Data2, Data3);
            if (breq == 1){
                bat_runtime_eval(bat, group, line, offset + 4);
            }
            bat_debug("breq: %d | xret\n", breq);



            //bat_runtime_echo(TextTok->value);
            // continue;
        }
    }
    return 0;
}

int bat_runtime_exec(BAT_T* bat){
    bat_debug("========================\n");
    bat_debug("Runtime EXEC\n");
    bat_debug("========================\n");
    int ret = 0;
    for (int x = 0; x < bat->Size; x++){
        BAT_GROUP_T* group = (BAT_GROUP_T*) bat->Group[x];

        bat_runtime_echo(bat, group);

        ret = bat_runtime_eval(bat, group, x + 1, 0);
        bat_debug("line: %d | ret: %d\n", x +1, ret);
        if (EBAT_CONFIG_CRITICAL_STOP == 1 && ret > 0) {
            bat_debug("CRITICAL OUT\n");
            //bat->ErrorCode = ret;
            break;
        }

    }
    bat->ErrorCode = ret;
    bat_debug("========================\n");
    return ret;
}
