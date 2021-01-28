let srcpathManager = {}; 

srcpathManager.commonPath = ""; 

srcpathManager.cleanDoubleDots = function(filePath) {
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


srcpathManager.commonPathDetection = function () {
    let commonPath = srcpathManager.commonPath;
    return function (filePath) {
        filePath = srcpathManager.cleanDoubleDots(filePath);
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


srcpathManager.findCommonPath = function(filePaths) {
    for(let filePath of filePaths){
        filePath = srcpathManager.cleanDoubleDots(filePath);
        if (srcpathManager.commonPath === "")
        srcpathManager.commonPath = filePath;
        else {
            let i = 0;
            let minLength = srcpathManager.commonPath.length < filePath.length ? srcpathManager.commonPath.length : filePath.length;
            while (i < minLength && srcpathManager.commonPath.charAt(i) === filePath.charAt(i)) i++;
            srcpathManager.commonPath = srcpathManager.commonPath.substring(0, i);
        }
    }
    return srcpathManager.commonPath;
}


srcpathManager.getCleanFileName = function(fullFileName) {
    fullFileName = srcpathManager.cleanDoubleDots(fullFileName);
    return fullFileName.substring(srcpathManager.commonPath.length, fullFileName.length);
}


export default srcpathManager;