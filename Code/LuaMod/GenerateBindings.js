let classes = [];
let current_class_name = '';
let current_class_functions = [];
let public_section = false;
let fs = require('fs');

function generateFromDir(directory, on_finished) {
    let generatedFileContent = '';

    fs.readdir(directory, function(err, items) {
        let itemIdx = 0;

        let hppFiles = [];
        items.forEach((item)=> {
            if(item.indexOf('.hpp') >-1) 
                hppFiles.push(item);
        });

        hppFiles.forEach((item)=> {

            console.log('[Lua Bindings Generator] creating bindings for: ' + item + '..');
            generateFromFile(directory + '\\' + item, (content)=> {
                generatedFileContent += content;

                if(hppFiles.length - 1 == itemIdx) {
                    console.log('[Lua Bindings Generator] bindings for: ' + item + ' created.');
                    on_finished(generatedFileContent);
                }
                itemIdx++;
            });
        });
    });
}

function generateFromFile(file, on_finished) {
    var lineReader = require('readline').createInterface({
        input: fs.createReadStream(file)
    });

    lineReader.on('line', function (line) {

        let className = extractClassName(line);
        if(className != '') {
            if(current_class_name != '') {
                classes.push({
                    name: current_class_name,
                    functions: current_class_functions
                });
            }
            current_class_functions = [];
            current_class_name = className;
            public_section = false;
        }
        
        if(line.indexOf('private:') > -1) {
            public_section = false;
            return;
        }
        
        if(line.indexOf('public:') > -1) {
            public_section = true;
            return;
        }
    
        if(!public_section) return '';

        if(line.indexOf('(') > -1 && line.indexOf(')') > -1) {
            let name = extractFunctionName(line);
            if(name != '') {
                current_class_functions.push(name);
            }
        }
    });
    
    lineReader.on('close', function() {
        
        if(current_class_functions.length) {
            classes.push({
                name: current_class_name,
                functions: current_class_functions
            });
        }
    
        if(on_finished != null)  {
            on_finished(generateBindings());
            classes = [];
            current_class_functions = [];
            current_class_name = '';
            public_section = false;
        }
    });
}

function extractClassName(line) {
    let className = '';
    if(line.indexOf('class') > -1) {
        let afterClassName = line.split('class ')[1];

        for(let i = 0; i < afterClassName.length; i++) {
            if(afterClassName[i] == ' ' || afterClassName[i] == '{' || afterClassName[i] == '\n')
                break;
            
            className += afterClassName[i];
        }
    } 

    return className;
}

function extractFunctionName(line) {

    let beforeFunctionBracket = line.split('(')[0];
    if(beforeFunctionBracket.indexOf(' ') > -1 ) {
        let functionName = '';
        let startPicking = false;
        for(let i = 0; i < beforeFunctionBracket.length; i++) {
            if(beforeFunctionBracket[i - 1] != ' ' && 
               beforeFunctionBracket[i] == ' '     && 
               beforeFunctionBracket[i + 1] != ' ') 
            startPicking = true;

            if(startPicking)
                functionName += beforeFunctionBracket[i];
        }

        return functionName;
    }
}

function generateBindings() {

    let returnString = '';
    classes.forEach((currentClass)=> {
        returnString += 'lua.new_usertype<'  + currentClass.name + '>("' + currentClass.name + '",\n';
		
        let currentFunctionIdx = 0
        currentClass.functions.forEach((functionName)=> {
            
            if(currentClass.functions.length - 1 == currentFunctionIdx) 
                returnString += '\tBIND_FUNCTION(' + currentClass.name + ', ' + functionName + ')\n);\n\n';
            else
                returnString += '\tBIND_FUNCTION(' + currentClass.name + ', ' + functionName + '),\n';
            
            currentFunctionIdx++;
        });
    });

    return returnString;
}

generateFromDir('../Shared/Oakwood', (generatedFile)=> {
  
    fs.writeFile("LuaBindings.hpp", generatedFile, function(err) {
        if(err) {
            return console.log(err);
        }
        console.log("[Lua Bindings Generator] bingings successfully created !");
    }); 
});