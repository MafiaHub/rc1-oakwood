#define ZPL_DEFINE
#include "librg/zpl.h"

enum {
    OAK_MODE_FORWARD,
    OAK_MODE_VTABLE,
    OAK_MODE_TYPES,
    OAK_MODE_ASSIGN,
};

zpl_global char *mode_pat[] = {
    "forward",
    "generate_vtable",
    "generate_types",
    "generate_assign"
};

enum {
    FORWARD,
    NATIVE,
    PROTO_DEFAULT,
    NAMESPACE
};

zpl_global char *syms[] = {
    "FORWARD",
    "NATIVE",
    "PROTO_DEFAULT",
    "NAMESPACE"
};

i32 get_index_from_name(char *str, isize len)
{
    for (isize i = 0; i < zpl_count_of(syms); i += 1){
        if (!zpl_strncmp(syms[i], str, len))
        {
            return (i32)i;
        }
    }
    
    return -1;
}

char *process_arglist(char *src, char ***args)
{
    char *p = src + 1;
    
    while (*p)
    {
        p = zpl_str_trim(p, true);
        
        char *tb = p;
        char *te = p;
        
        while (*p && *p != ',' && *p != ')')
        {
            p++;
        }
        
        te = p;
        
        if (te-tb == 0)
            return p;
        
        char *arg = (char *)zpl_malloc(te-tb + 1);
        zpl_strncpy(arg, tb, te-tb);
        
        zpl_array_append(*args, arg);
        
        if (*p == ')')
            return p;
        
        p = te+1;
    }
    
    return p;
}

void process_string(char *src, i32 mode, zpl_file *file)
{
    char *b = src;
    char *p = src;
    
    while (*p) 
    {
        p = zpl_str_trim(p, false);
        
        p = strstr(p, "OAKGEN_");
        
        if (!p) 
            return;
        
        p += 7;
        char *op = p;
        
        char *fb = p;
        char *fe = p;
        
        fe = (char *)zpl_char_first_occurence(p, '(');
        
        i32 index = get_index_from_name(p, (fe-fb));
        
        if (index == -1)
        {
            p++;
            continue;
        }
        
        b = fe;
        
        zpl_array_make(char*, args, zpl_heap());
        p = process_arglist(b, &args) + 1;
        
        if (*p == ';') p++;
        p = zpl_str_trim(p, false);
        
        switch (index)
        {
            case FORWARD: {
                char *glue_code = ";";
                char *tb = p;
                char *te = (char *)zpl_char_first_occurence(p, '{');
                
                zpl_file_write(file, (void *)tb, te-tb);
                zpl_file_write(file, (void *)zpl_str_expand(glue_code));
            } break;
            
            case NATIVE: {
                char *glue_code = ";";
                char *te = (char *)zpl_char_first_occurence(p, '{');
                
                zpl_string signature = zpl_string_make_length(zpl_heap(),
                                                              (char *)p,
                                                              te-p);
                
                char *tpb = p = zpl_str_trim(p, false);
                char *tpe = p = (char *)zpl_char_first_occurence(p, ' ');
                
                zpl_string type_name = zpl_string_make_length(zpl_heap(),
                                                              tpb,
                                                              tpe-tpb);
                
                p = zpl_str_trim(p, false);
                
                te = (char *)zpl_char_first_occurence(p, '(');
                char *arge = (char *)zpl_char_first_occurence(te, ')')+1;
                zpl_string arguments = zpl_string_make_length(zpl_heap(), te, arge-te);
                
                zpl_string method_name = zpl_string_make_length(zpl_heap(),
                                                                (char *)p,
                                                                te-p);
                
                zpl_string macro_name = zpl_string_duplicate(zpl_heap(), method_name);
                
                zpl_str_to_upper(macro_name);
                
                switch (mode)
                {
                    case OAK_MODE_TYPES: {
                        zpl_string line = zpl_string_sprintf_buf(zpl_heap(),
                                                                 "#define %s(name) %s name%s\n",
                                                                 macro_name,
                                                                 type_name,
                                                                 arguments);
                        
                        zpl_string line2 = zpl_string_sprintf_buf(zpl_heap(),
                                                                  "typedef %s(%s_ptr);\n\n",
                                                                  macro_name,
                                                                  method_name);
                        
                        zpl_file_write(file, (void *)zpl_str_expand(line));
                        zpl_file_write(file, (void *)zpl_str_expand(line2));
                    } break;
                    
                    case OAK_MODE_VTABLE: {
                        zpl_string line = zpl_string_sprintf_buf(zpl_heap(),
                                                                 "%s_ptr *%s;\n",
                                                                 method_name,
                                                                 method_name+4);
                        
                        zpl_file_write(file, (void *)zpl_str_expand(line));
                    } break;
                    
                    case OAK_MODE_ASSIGN: {
                        zpl_string line = zpl_string_sprintf_buf(zpl_heap(),
                                                                 "vt->%s = %s;\n",
                                                                 method_name+4,
                                                                 method_name);
                        
                        zpl_file_write(file, (void *)zpl_str_expand(line));
                    } break;
                }
            } break;
        }
        
        p = op;
    }
}

i32 get_mode_from_name(char *name)
{
    zpl_str_to_lower(name);
    
    for (i32 i = 0; i < zpl_count_of(mode_pat); i += 1){
        if (!zpl_strcmp(name, mode_pat[i]))
        {
            return i;
        }
    }
    
    return -1;
}

int main(int argc, char **argv)
{
    zpl_opts opts = {0};
    
    zpl_opts_init(&opts, zpl_heap(), argv[0]);
    
    zpl_opts_add(&opts, "m", "mode", "generation mode.", ZPL_OPTS_STRING);
    zpl_opts_add(&opts, "f", "file", "input file name.", ZPL_OPTS_STRING);
    
    zpl_opts_positional_add(&opts, "mode");
    zpl_opts_positional_add(&opts, "file");
    
    b32 ok = zpl_opts_compile(&opts, argc, argv);
    
    if (ok)
    {
        char *filename = zpl_opts_string(&opts, "file", NULL);
        char *mode = zpl_opts_string(&opts, "mode", NULL);
        
        if (!filename || !mode) 
        {
            zpl_opts_print_errors(&opts);
            zpl_opts_print_help(&opts);
            zpl_exit(3);
        }
        
        i32 mode_ind = get_mode_from_name(mode);
        
        if (mode_ind == -1)
        {
            zpl_printf("Mode unrecognized\n");
            zpl_exit(1);
        }
        
        zpl_file_contents input = zpl_file_read_contents(zpl_heap(),
                                                         true,
                                                         filename);
        
        process_string((char *)input.data, 
                       mode_ind,
                       zpl_file_get_standard(ZPL_FILE_STANDARD_OUTPUT));
    }
    else
    {
        zpl_printf("It's broken\n");
        zpl_exit(2);
    }
    
    return 0;
}
