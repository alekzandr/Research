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

