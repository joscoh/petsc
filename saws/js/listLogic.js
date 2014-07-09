/*
  This function is called when a pc_type option is changed (new options may need to be displayed and/or old ones removed
*/

$(document).on("change","select[id^='pc_type']",function() {

    //get the pc option
    var pcValue   = $(this).val();
    var id        = $(this).attr("id");//really should not be used in this method. there are better ways of getting information
    var endtag    = id.substring(id.indexOf("0"),id.length);
    var parentDiv = "solver" + endtag;
    var index     = getIndex(matInfo,endtag);

    removeAllChildren(endtag);//this function also changes matInfo as needed

    if (pcValue == "mg") {
        var defaultMgLevels = 2;

        matInfo[index].pc_type      = "mg";
        matInfo[index].pc_mg_levels = defaultMgLevels;

        //first add options related to multigrid (pc_mg_type and pc_mg_levels)
        $("#" + parentDiv).append("<br><b>MG Type &nbsp;&nbsp;</b><select id=\"pc_mg_type" + endtag + "\"></select>");
        $("#" + parentDiv).append("<br><b>MG Levels </b><input type='text' id=\'pc_mg_levels" + endtag + "\' maxlength='4'>");
        $("#pc_mg_levels" + endtag).val(defaultMgLevels);

        populateMgList(endtag);

        //display options for each level
        for(var i=defaultMgLevels-1; i>=0; i--) {
            var childEndtag = endtag + "_" + i;

            var writeLoc = matInfo.length;
            matInfo[writeLoc] = {
                pc_type : "sor",
                ksp_type: "chebyshev",
                endtag : childEndtag
            }

            var margin = 30 * getNumUnderscores(childEndtag);  //indent based on the level of the solver (number of underscores)

            $("#" + parentDiv).after("<div id=\"solver" + childEndtag + "\" style=\"margin-left:" + margin + "px;\"></div>");
            if(i == 0) //coarse grid solver (level 0)
                $("#solver" + childEndtag).append("<br><b>Coarse Grid Solver (Level 0)  </b>");
            else
                $("#solver" + childEndtag).append("<br><b>Smoothing (Level " + i + ")  </b>");

            $("#solver" + childEndtag).append("<br><b>KSP &nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"ksp_type" + childEndtag  + "\"></select>");
	    $("#solver" + childEndtag).append("<br><b>PC  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"pc_type" + childEndtag + "\"></select>");

            populateKspList(childEndtag);
            populatePcList(childEndtag);

	    //set defaults
	    $("#ksp_type" + childEndtag).find("option[value='chebyshev']").attr("selected","selected");
	    $("#pc_type" + childEndtag).find("option[value='sor']").attr("selected","selected");
        }

    }

    else if (pcValue == "redundant") {
        var defaultRedundantNumber = 2;
        var childEndtag = endtag + "_0";

        matInfo[index].pc_type             = "redundant";
        matInfo[index].pc_redundant_number = defaultRedundantNumber;

        //first add options related to redundant (pc_redundant_number)
        $("#" + parentDiv).append("<br><b>Redundant Number </b><input type='text' id=\'pc_redundant_number" + endtag + "\' maxlength='4'>");
        $("#pc_redundant_number" + endtag).val(defaultRedundantNumber);

        var writeLoc = matInfo.length;
        matInfo[writeLoc] = {
            pc_type : "",
            ksp_type: "preonly",
            endtag : childEndtag,
            symm: false //perhaps should inherit this val from parent? (ask dr. smith)
        }

        var margin = 30 * getNumUnderscores(childEndtag);  //indent based on the level of the solver (number of underscores)
	$("#" + parentDiv).after("<div id=\"solver" + childEndtag + "\" style=\"margin-left:" + margin + "px;\"></div>");
	$("#solver" + childEndtag).append("<br><b>Redundant Solver Options </b>");
	$("#solver" + childEndtag).append("<br><b>KSP    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"ksp_type" + childEndtag + "\"></select>");
	$("#solver" + childEndtag).append("<br><b>PC     &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"pc_type" + childEndtag + "\"></select>");

	populateKspList(childEndtag);
        populatePcList(childEndtag);

	//set defaults for redundant
	$("#ksp_type" + childEndtag).find("option[value='preonly']").attr("selected","selected");
        if (matInfo[index].symm) {
            $("#pc_type" + childEndtag).find("option[value='cholesky']").attr("selected","selected");
            matInfo[writeLoc].pc_type = "cholesky";
        } else {
	    $("#pc_type" + childEndtag).find("option[value='lu']").attr("selected","selected");
            matInfo[writeLoc].pc_type = "lu";
        }
    }

    else if (pcValue == "bjacobi") {

        var defaultBjacobiBlocks = 2;
        var childEndtag = endtag + "_0";

        matInfo[index].pc_type             = "bjacobi";
        matInfo[index].pc_bjacobi_blocks   = defaultBjacobiBlocks;

        //first add options related to bjacobi (pc_bjacobi_blocks)
        $("#" + parentDiv).append("<br><b>Bjacobi Blocks </b><input type='text' id=\'pc_bjacobi_blocks" + endtag + "\' maxlength='4'>");
        $("#pc_bjacobi_blocks" + endtag).val(defaultBjacobiBlocks);

        var writeLoc = matInfo.length;
        matInfo[writeLoc] = { //here, we should set default symm/posdef/logstruc according to parent properties
            pc_type : "",
            ksp_type: "preonly",
            endtag : childEndtag,
            symm: false
        }

        var margin = 30 * getNumUnderscores(childEndtag);  //indent based on the level of the solver (number of underscores)

	$("#" + parentDiv).after("<div id=\"solver" + childEndtag + "\" style=\"margin-left:" + margin + "px;\"></div>");

	$("#solver" + childEndtag).append("<br><b>Bjacobi Solver Options </b>");
	$("#solver" + childEndtag).append("<br><b>KSP    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"ksp_type" + childEndtag + "\"></select>");
	$("#solver" + childEndtag).append("<br><b>PC     &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"pc_type" + childEndtag + "\"></select>");

	populateKspList(childEndtag);
        populatePcList(childEndtag);

	//set defaults for bjacobi
	$("#ksp_type" + childEndtag).find("option[value='preonly']").attr("selected","selected");
        if (matInfo[index].symm) {
            $("#pc_type" + childEndtag).find("option[value='icc']").attr("selected","selected");
            matInfo[writeLoc].pc_type = "icc";
        } else {
	    $("#pc_type" + childEndtag).find("option[value='ilu']").attr("selected","selected");
            matInfo[writeLoc].pc_type = "ilu";
        }
    }

    else if (pcValue == "asm") {

        var defaultAsmBlocks  = 2;
        var defaultAsmOverlap = 2;
        var childEndtag = endtag + "_0";

        matInfo[index].pc_type             = "asm";
        matInfo[index].pc_asm_blocks       = defaultAsmBlocks;
        matInfo[index].pc_asm_overlap      = defaultAsmOverlap;

        //first add options related to ASM
        $("#" + parentDiv).append("<br><b>ASM blocks   &nbsp;&nbsp;</b><input type='text' id=\"pc_asm_blocks" + endtag + "\" maxlength='4'>");
	$("#" + parentDiv).append("<br><b>ASM overlap   </b><input type='text' id=\"pc_asm_overlap" + endtag + "\" maxlength='4'>");
        $("#pc_asm_blocks" + endtag).val(defaultAsmBlocks);
        $("#pc_asm_overlap" + endtag).val(defaultAsmOverlap);

        var writeLoc = matInfo.length;
        matInfo[writeLoc] = {
            pc_type : "",
            ksp_type: "preonly",
            endtag : childEndtag,
            symm: false
        }

        var margin = 30 * getNumUnderscores(childEndtag);  //indent based on the level of the solver (number of underscores)

	$("#" + parentDiv).after("<div id=\"solver" + childEndtag + "\" style=\"margin-left:" + margin + "px;\"></div>");

	$("#solver" + childEndtag).append("<br><b>ASM Solver Options </b>");
	$("#solver" + childEndtag).append("<br><b>KSP    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"ksp_type" + childEndtag + "\"></select>");
	$("#solver" + childEndtag).append("<br><b>PC     &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"pc_type" + childEndtag + "\"></select>");

	populateKspList(childEndtag);
        populatePcList(childEndtag);

	//set defaults for asm
	$("#ksp_type" + childEndtag).find("option[value='preonly']").attr("selected","selected");
        if (matInfo[index].symm) {
            $("#pc_type" + childEndtag).find("option[value='icc']").attr("selected","selected");
            matInfo[writeLoc].pc_type = "icc";
        } else {
	    $("#pc_type" + childEndtag).find("option[value='ilu']").attr("selected","selected");
            matInfo[writeLoc].pc_type = "ilu";
        }
    }

    else if (pcValue == "ksp") {
        var childEndtag = endtag + "_0";

        matInfo[index].pc_type             = "ksp";

        var writeLoc = matInfo.length;
        matInfo[writeLoc] = {
            pc_type : "bjacobi",
            ksp_type: "gmres",
            endtag : childEndtag,
            symm: false
        }

        var margin = 30 * getNumUnderscores(childEndtag);  //indent based on the level of the solver (number of underscores)

	$("#" + parentDiv).after("<div id=\"solver" + childEndtag + "\" style=\"margin-left:" + margin + "px;\"></div>");

	$("#solver" + childEndtag).append("<br><b>KSP Solver Options </b>");
	$("#solver" + childEndtag).append("<br><b>KSP    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"ksp_type" + childEndtag + "\"></select>");
	$("#solver" + childEndtag).append("<br><b>PC     &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"pc_type" + childEndtag + "\"></select>");

	populateKspList(childEndtag);
        populatePcList(childEndtag);

	//set defaults for ksp
	$("#ksp_type" + childEndtag).find("option[value='gmres']").attr("selected","selected");
	$("#pc_type" + childEndtag).find("option[value='bjacobi']").attr("selected","selected");
	//bjacobi has extra dropdown menus so manually trigger once
	$("#pc_type" + childEndtag).trigger("change");
    }

    else if (pcValue == "fieldsplit") {//just changed to fieldsplit so set up defaults (2 children)

        var defaultFieldsplitBlocks = 2;

        matInfo[index].pc_type              = "fieldsplit";
        matInfo[index].pc_fieldsplit_type   = "multiplicative";//this is hardcoded
        matInfo[index].pc_fieldsplit_blocks = defaultFieldsplitBlocks;

        //first add options related to fieldsplit (pc_fieldsplit_type and pc_fieldsplit_blocks)
        $("#" + parentDiv).append("<br><b>Fieldsplit Type &nbsp;&nbsp;</b><select id=\"pc_fieldsplit_type" + endtag + "\"></select>");
        $("#" + parentDiv).append("<br><b>Fieldsplit Blocks </b><input type='text' id=\"pc_fieldsplit_blocks" + endtag + "\" maxlength='4'>");
        $("#pc_fieldsplit_blocks" + endtag).val(defaultFieldsplitBlocks);

        populateFieldsplitList(endtag);

        for(var i=defaultFieldsplitBlocks-1; i>=0; i--) {
            var childEndtag = endtag + "_" + i;

            var writeLoc = matInfo.length;
            matInfo[writeLoc] = {
                pc_type : "redundant",
                ksp_type: "preonly",
                endtag : childEndtag
            }

            var margin = 30 * getNumUnderscores(childEndtag);  //indent based on the level of the solver (number of underscores)

            $("#" + parentDiv).after("<div id=\"solver" + childEndtag + "\" style=\"margin-left:" + margin + "px;\"></div>");
            $("#solver" + childEndtag).append("<br><b>Fieldsplit " + (i+1) + " Options</b>");
            $("#solver" + childEndtag).append("<br><b>KSP &nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"ksp_type" + childEndtag  + "\"></select>");
	    $("#solver" + childEndtag).append("<br><b>PC  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"pc_type" + childEndtag + "\"></select>");

            populateKspList(childEndtag);
            populatePcList(childEndtag);

	    //set defaults
	    $("#ksp_type" + childEndtag).find("option[value='preonly']").attr("selected","selected");
	    $("#pc_type" + childEndtag).find("option[value='redundant']").attr("selected","selected");
	    //redundant has to have extra dropdown menus so manually trigger
	    $("#pc_type" + childEndtag).trigger("change");
        }
    }

    else { //for other pcs, simply adjust pc_type in matInfo
        matInfo[index].pc_type = pcValue;
    }
});

//called when a ksp option is changed
//simply adjust ksp_type in matInfo
$(document).on("change","select[id^='ksp_type']",function() {

    var kspValue   = $(this).val();
    var id         = $(this).attr("id");//really should not be used in this method. there are better ways of getting information
    var endtag     = id.substring(id.indexOf("0"),id.length);
    var index      = getIndex(matInfo,endtag);

    matInfo[index].ksp_type = kspValue;
});

//need to add a bunch of methods here for changing each variable: pc_fieldsplit_blocks, pc_asm_blocks, pc_redundant_number, etc
//$(document).on("change","input


//input: endtag of the parent
function removeAllChildren(endtag) {

    var index       = getIndex(matInfo, endtag);
    var numChildren = getNumChildren(matInfo, endtag);

    for(var i=0; i<numChildren; i++) {
        var childEndtag = endtag + "_" + i;
        var childIndex  = getIndex(matInfo,childEndtag);

        if(getNumChildren(matInfo, childEndtag) > 0)//this child has more children
        {
            removeAllChildren(childEndtag);//recursive call to remove all children of that child
        }
        matInfo[childIndex].endtag = "-1";//make sure this location is never accessed again.

        $("#solver" + childEndtag).remove();//remove that child itself
    }

    //adjust variables in matInfo
    if(matInfo[index].pc_type == "mg") {
        matInfo[index].pc_mg_levels = 0;
    }
    else if(matInfo[index].pc_type == "fieldsplit") {
        matInfo[index].pc_fieldsplit_blocks = 0;
    }

    $("#pc_type" + endtag).nextAll().remove();//remove the options in the same level solver

}

//called when text input for pc_fieldsplit_blocks is changed
$(document).on('keyup', "input[id^='pc_fieldsplit_blocks']", function() {

    if($(this).val().match(/[^0-9]/) || $(this).val()<1) //return on invalid input
        return;

    var id     = this.id;
    var endtag = id.substring(id.indexOf(0),id.length);
    var index  = getIndex(matInfo, endtag);
    var val    = $(this).val();

    // this next part is a bit tricky...there are 2 cases

    //case 1: we need to remove some divs
    if(val < matInfo[index].pc_fieldsplit_blocks) {
        for(var i=val; i<matInfo[index].pc_fieldsplit_blocks; i++) {
            var childEndtag = endtag + "_" + i;
            var childIndex  = getIndex(matInfo, childEndtag);

            removeAllChildren(childEndtag); //remove grandchildren (if any)
            matInfo[childIndex].endtag = "-1"; //set matInfo endtag to "-1"
            $("#solver" + childEndtag).remove(); //remove the divs
        }
        matInfo[index].pc_fieldsplit_blocks = val;
    }

    //case 2: we need to add some divs
    else if(val > matInfo[index].pc_fieldsplit_blocks) {
        for(var i = matInfo[index].pc_fieldsplit_blocks; i < val; i++) {

            //add divs and write matInfo
            var childEndtag = endtag + "_" + i;
            var margin = getNumUnderscores(childEndtag) * 30;

            //this is the trickiest part: need to find exactly where to insert the new divs
            //find the first div that doesn't begin with endtag

            var currentDiv  = $(this).parent().get(0);

            while($(currentDiv).next().length > 0) { //while has next
                var nextDiv    = $(currentDiv).next().get(0);
                var nextId     = nextDiv.id;
                var nextEndtag = nextDiv.id.substring(nextId.indexOf("0"),nextId.length);

                if(nextEndtag.indexOf(endtag) == 0) {
                    currentDiv = nextDiv;
                }
                else
                    break;
            }

            //append new stuff immediately after current div
            var writeLoc = matInfo.length;
            matInfo[writeLoc] = {
                pc_type : "redundant",
                ksp_type: "preonly",
                endtag : childEndtag
            }

            var margin = 30 * getNumUnderscores(childEndtag);  //indent based on the level of the solver (number of underscores)
            $(currentDiv).after("<div id=\"solver" + childEndtag + "\" style=\"margin-left:" + margin + "px;\"></div>");
            $("#solver" + childEndtag).append("<br><b>Fieldsplit " + (i+1) + " Options</b>");
            $("#solver" + childEndtag).append("<br><b>KSP &nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"ksp_type" + childEndtag  + "\"></select>");
	    $("#solver" + childEndtag).append("<br><b>PC  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"pc_type" + childEndtag + "\"></select>");

            populateKspList(childEndtag);
            populatePcList(childEndtag);

	    //set defaults
	    $("#ksp_type" + childEndtag).find("option[value='preonly']").attr("selected","selected");
	    $("#pc_type" + childEndtag).find("option[value='redundant']").attr("selected","selected");
	    //redundant has to have extra dropdown menus so manually trigger
	    $("#pc_type" + childEndtag).trigger("change");
        }
        matInfo[index].pc_fieldsplit_blocks = val;
    }
});

/*
  This function is called when the text input "MG Levels" is changed
*/
$(document).on('keyup', "input[id^='pc_mg_levels']", function()
{
    if($(this).val().match(/[^0-9]/) || $(this).val()<1) //return on invalid input
        return;

    var id     = this.id;
    var endtag = id.substring(id.indexOf(0),id.length);
    var index  = getIndex(matInfo, endtag);
    var val    = $(this).val();

    // this next part is a bit tricky...there are 2 cases

    //case 1: we need to remove some divs
    if(val < matInfo[index].pc_mg_levels) {
        for(var i=val; i<matInfo[index].pc_mg_levels; i++) {
            var childEndtag = endtag + "_" + i;
            var childIndex  = getIndex(matInfo, childEndtag);

            removeAllChildren(childEndtag); //remove grandchildren (if any)
            matInfo[childIndex].endtag = "-1"; //set matInfo endtag to "-1"
            $("#solver" + childEndtag).remove(); //remove the divs
        }
        matInfo[index].pc_mg_levels = val;
    }

    //case 2: we need to add some divs
    else if(val > matInfo[index].pc_mg_levels) {
        for(var i = matInfo[index].pc_mg_levels; i < val; i++) {
            var childEndtag = endtag + "_" + i;
            var margin = getNumUnderscores(childEndtag) * 30;

            //this is the trickiest part: need to find exactly where to insert the new divs
            //find the first div that doesn't begin with endtag

            var currentDiv  = $(this).parent().get(0);

            while($(currentDiv).next().length > 0) { //while has next
                var nextDiv    = $(currentDiv).next().get(0);
                var nextId     = nextDiv.id;
                var nextEndtag = nextDiv.id.substring(nextId.indexOf("0"),nextId.length);

                if(nextEndtag.indexOf(endtag) == 0) {
                    currentDiv = nextDiv;
                }
                else
                    break;
            }

            //append new stuff immediately after current div
            var writeLoc = matInfo.length;
            matInfo[writeLoc] = {
                pc_type : "sor",
                ksp_type: "chebyshev",
                endtag : childEndtag
            }

            var margin = 30 * getNumUnderscores(childEndtag);  //indent based on the level of the solver (number of underscores)
            $(currentDiv).after("<div id=\"solver" + childEndtag + "\" style=\"margin-left:" + margin + "px;\"></div>");
            if(i == 0) //coarse grid solver (level 0)
                $("#solver" + childEndtag).append("<br><b>Coarse Grid Solver (Level 0)  </b>");
            else
                $("#solver" + childEndtag).append("<br><b>Smoothing (Level " + i + ")  </b>");

            $("#solver" + childEndtag).append("<br><b>KSP &nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"ksp_type" + childEndtag  + "\"></select>");
	    $("#solver" + childEndtag).append("<br><b>PC  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><select id=\"pc_type" + childEndtag + "\"></select>");

            populateKspList(childEndtag);
            populatePcList(childEndtag);

            //set defaults
            $("#ksp_type" + childEndtag).find("option[value='chebyshev']").attr("selected","selected");
            $("#pc_type" + childEndtag).find("option[value='sor']").attr("selected","selected");
        }
        matInfo[index].pc_mg_levels = val;
    }
});