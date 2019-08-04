const fs = require('fs')
const path = require('path')

const fntemplate = `
const char *oak__methods() {
    return "METHOD_DATA";
}

int oak_bridge_router(const char *nanobuffer, usize size) {
    cw_pack_context opc;
    cw_unpack_context ipc;
    int errcode = 0;
    char *errstr = "";

    char buffer[OAK_BRIDGE_BUFFER] = {};

    cw_pack_context_init(&opc, buffer, OAK_BRIDGE_BUFFER, 0);
    cw_unpack_context_init(&ipc, (void *)nanobuffer, size, 0);

    cw_unpack_next(&ipc);

    if (ipc.item.type != CWP_ITEM_ARRAY) {
        errcode = -1;
        errstr = zpl_bprintf("Error: the payload is invalid");
        goto oak_bridge_router_error;
    }

    cw_unpack_next(&ipc);

    char fname[256] = {};

    if (ipc.item.type != CWP_ITEM_STR) {
        errcode = -1;
        errstr = zpl_bprintf("Error: function name was not found");
        goto oak_bridge_router_error;
    }

    zpl_memcopy(fname, (const char *)ipc.item.as.str.start, (usize)ipc.item.as.str.length);

    TEMPLATE

    errcode = -1;
    errstr = zpl_bprintf("Error: function name was not found");

oak_bridge_router_error:
    cw_pack_array_size(&opc, 2);
    cw_pack_signed(&opc, errcode);
    cw_pack_str(&opc, errstr, zpl_strlen(errstr));

    nn_send(sock_in, buffer, opc.current - opc.start, 0);

    return -1;
}\n`

const main = async () => {
    const file = path.join(__dirname, '../Code/Server/oak_server.h')
    const data = fs.readFileSync(file, 'utf8')

    console.log('[bridge] starting the script')

    /* get dictionary with types from OAK_API_TYPE macros */
    const types = data
        .split('\n')
        .filter(line => line.indexOf('OAK_API_TYPE(') !== -1)
        .map(l => l.replace('OAK_API_TYPE', '').trim())
        .map(l => l.replace('(', ''))
        .map(l => l.replace(')', ''))
        .map(l => l.trim())
        .map(l => l.split(',').map(a => a.trim()))
        .reduce((c, l) => Object.assign(c, {[l[0]]: l[1]}), {})

    console.log(`[bridge] parsed ${Object.values(types).length} custom types`)

    /* get table with function signatures */
    const fns = data
        .split('\n')
        .filter(line => line.indexOf('OAK_API ') !== -1)
        .filter(line => line.indexOf('#define') === -1)
        .map(l => l.replace(/OAK_API[\s+]/, ''))
        .map(l => l.replace(';',''))
        .map(l => /(\w+)\s([a-z0-9_]+)\(([\w, ]*)\)/.exec(l))
        .filter(l => l)
        .map(l => [
            l[1].trim(),
            l[2].trim(),
            l[3].split(',').map(v => v.trim())
        ])

    console.log(`[bridge] parsed ${fns.length} functions`)

    fns.push(['oak_string', 'oak__methods', []])

    const checkarg = (type, msgpacktype) => `
        if (ipc.item.type != ${msgpacktype}) {
            errcode = -3;
            errstr = zpl_bprintf("Error: argument type mismatch; Expected '${type}', got '%d'", ipc.item.type);
            goto oak_bridge_router_error;
        }
    `

    const result = fns.map(fn => {
        const [ret, name, args] = fn

        const argline = args.map((a, i) => {
            let instruction = `
                cw_unpack_next(&ipc);`

            switch (types[a]) {
                case 'float':
                    instruction += `
                        float arg${i} = 0.0f;
                        if (ipc.item.type == CWP_ITEM_FLOAT)
                            arg${i} = ipc.item.as.real;
                        else if (ipc.item.type == CWP_ITEM_DOUBLE)
                            arg${i} = ipc.item.as.long_real;
                    `; break;
                case 'vec3':
                    instruction += `
                        oak_vec3 arg${i} = {};
                        ${checkarg('vec3', 'CWP_ITEM_ARRAY')}

                        cw_unpack_next(&ipc);
                        if (ipc.item.type == CWP_ITEM_FLOAT)
                            arg${i}.x = ipc.item.as.real;
                        else if (ipc.item.type == CWP_ITEM_DOUBLE)
                            arg${i}.x = ipc.item.as.long_real;

                        cw_unpack_next(&ipc);
                        if (ipc.item.type == CWP_ITEM_FLOAT)
                            arg${i}.y = ipc.item.as.real;
                        else if (ipc.item.type == CWP_ITEM_DOUBLE)
                            arg${i}.y = ipc.item.as.long_real;

                        cw_unpack_next(&ipc);
                        if (ipc.item.type == CWP_ITEM_FLOAT)
                            arg${i}.z = ipc.item.as.real;
                        else if (ipc.item.type == CWP_ITEM_DOUBLE)
                            arg${i}.z = ipc.item.as.long_real;
                    `; break;
                case 'str':
                    instruction += `
                        oak_string arg${i};
                        ${checkarg('str', 'CWP_ITEM_STR')}
                        arg${i} = (const char *)ipc.item.as.str.start;
                    `; break;
                default:
                    instruction += `
                        int arg${i} = 0;
                        if (ipc.item.type == CWP_ITEM_FLOAT)
                            arg${i} = ipc.item.as.real;
                        else if (ipc.item.type == CWP_ITEM_DOUBLE)
                            arg${i} = ipc.item.as.long_real;
                        else if (ipc.item.type == CWP_ITEM_NEGATIVE_INTEGER)
                            arg${i} = ipc.item.as.i64;
                        else if (ipc.item.type == CWP_ITEM_POSITIVE_INTEGER)
                            arg${i} = ipc.item.as.u64;
                        else if (ipc.item.type == CWP_ITEM_BOOLEAN)
                            arg${i} = ipc.item.as.boolean;
                    `; break;
            }

            return instruction
                .replace(/[ ]{20}/g, ' '.repeat(8))
                .replace(/[ ]{24}/g, ' '.repeat(12))
                .replace(/[ ]{16}/g, ' '.repeat(12))
        })

        let resline;

        switch (types[ret]) {
            case 'float': resline = `cw_pack_float(&opc, res);`; break;
            case 'str': resline = `cw_pack_str(&opc, res, zpl_strlen(res));`; break;
            case 'vec3': resline = `
                cw_pack_array_size(&opc, 3);
                cw_pack_float(&opc, res.x);
                cw_pack_float(&opc, res.y);
                cw_pack_float(&opc, res.z);
            `; break;
            default: resline = `cw_pack_signed(&opc, res);`; break;
        }

        return `
        if (!zpl_strncmp("${name}", fname, ${name.length})) {
            cw_unpack_next(&ipc);

            if (ipc.item.as.array.size != ${args.length}) {
                errcode = -2;
                errstr = zpl_bprintf("Error: argrument count invalid; Expected '${args.length}', got '%d'", ipc.item.as.array.size);
                goto oak_bridge_router_error;
            }

            ${argline.join('').trim()}

            ${ret} res = ${name}(${args.map((a, i) => `arg${i}`).join(', ')});

            cw_pack_array_size(&opc, 2);
            cw_pack_signed(&opc, 0);

            ${resline}
            nn_send(sock_in, buffer, opc.current - opc.start, 0);

            return 0;
        }`
    })

    const methodData = fns
        .map(v => v[1])
        .join(';')

    const fresult = fntemplate
        .replace('TEMPLATE', result.join('\n'))
        .replace('METHOD_DATA', methodData)


    fs.writeFileSync(path.join(
        __dirname,
        '../Code/Server/core/bridge.generated.h'),
    fresult)

    console.log('[bridge] succeeded')
}

main().catch(console.error)
