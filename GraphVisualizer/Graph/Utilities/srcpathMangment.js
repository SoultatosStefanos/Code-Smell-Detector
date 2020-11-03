
function cleanDoubleDots(filePath) {
    const regex = /\/|\\/g;
    const pathFolders = filePath.split(regex);

    for (let i = 0; i < pathFolders.length; ++i) {
        if (pathFolders[i] === '..') {
            pathFolders.splice(i - 1, 2);
            i -= 2;
        }
    }
    let cleanPath = "";
    pathFolders.forEach(folder => { cleanPath += folder + '\\' });

    return cleanPath.slice(0, -1);
}

let commonPathDetection = function () {
    let commonPath = "";
    return function (filePath) {
        if (commonPath === "")
            commonPath = filePath;
        else {
            let i = 0;
            let minLength = commonPath.length < filePath.length ? commonPath.length : filePath.length;
            while (i < minLength && commonPath.charAt(i) === filePath.charAt(i)) i++;
            commonPath = commonPath.substring(0, i);
        }
        return commonPath;
    }
}(); 

export default {
    cleanDoubleDots, 
    commonPathDetection
}