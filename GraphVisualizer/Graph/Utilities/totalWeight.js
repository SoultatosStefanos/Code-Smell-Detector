export default function totalWeight(dependencies, inData) {
    let dependenciesKeys = Object.keys(dependencies);
    let total = 0;
    dependenciesKeys.forEach((depKind) => {
        let data;
        if (inData !== undefined && (depKind.includes('Method') || depKind.includes('MemberExpr'))) {
            data = inData['Methods'];
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

