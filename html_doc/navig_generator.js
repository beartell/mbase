    let nvg= document.getElementById("pageNavigation");
    let firstList = document.createElement("ul");
    let sectionArray = [];
    function findAllSections(startString, associatedList) 
    {
        associatedList.style.listStyleType = "none";
        startString += 'c';
        for(let i = 0;;i++)
        {
            const sectionIdString = startString + i.toString();
            let foundSection = document.getElementById(sectionIdString);

            if(foundSection)
            {
                let newLi = document.createElement("li");
                let newAnchor = document.createElement("a");
                newAnchor.href = '#' + sectionIdString;
                newAnchor.innerHTML = foundSection.innerHTML;
                newLi.appendChild(newAnchor);
                associatedList.appendChild(newLi);
                
                let newOrderedList = document.createElement("ul");
                associatedList.appendChild(newOrderedList);
                sectionArray.push(foundSection.id);   
                findAllSections(sectionIdString, newOrderedList);             
                
            }
            else
            {
                break;
            }
        }
    }

    findAllSections("", firstList);
    if(sectionArray.length)
    {
        nvg.appendChild(firstList);
        nvg.style.visibility = "visible";
        
    }

