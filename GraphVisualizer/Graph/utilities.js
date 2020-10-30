export function totalWeight(dependencies, inData) {
    let dependenciesKeys = Object.keys(dependencies);
    let total = 0;
    dependenciesKeys.forEach((depKind) => {
        let data;
        if (inData !== undefined && (depKind.includes('Method') || depKind.includes('MemberExpr'))) {
            data = inData['Methods'];
            if (depKind.includes('MemberExpr')) {
                data = data['MemberExpr'];
            }
        }
        else
            data = inData;

        if (data === undefined || data[depKind] === undefined)
            total += dependencies[depKind];
        else if (data[depKind].custom === false) {
            if (data[depKind].countOnce === undefined || data[depKind].countOnce === false)
                total += dependencies[depKind];
            else
                total += 1;
        }
        else {
            if (data[depKind].countOnce === undefined || data[depKind].countOnce === false)
                total += data[depKind].weight * dependencies[depKind];
            else
                total += Number(data[depKind].weight);
        }
    });
    return Math.round((total + Number.EPSILON) * 100) / 100;
}


export function cleanDoubleDotsFromFilePath(filePath) {
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


export let CommonPath = function () {
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