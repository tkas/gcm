function init() {
	var sections = document.getElementsByClassName("foldsection");
	for (var i in sections) {
		var section = sections[i];
		var node = section.firstChild;
		var h1, content;
		while (node) {
			if (node.tagName == "H1") {
				h1 = node;
			}
			if (node.className && node.className.indexOf("content") >= 0) {
				content = node;
			}
			node = node.nextSibling;
		}
	
		if (h1 && content) {
			h1.addEventListener("click", (function(h1, content) { return function() {
				if (h1.parentNode.className.indexOf("expanded") >= 0) {
					h1.parentNode.className = "foldsection";
					content.style.display = "none";
				} else {
					h1.parentNode.className = "foldsection expanded";
					content.style.display = "";
				}
			}})(h1, content), false);
		}
	}
}

function rot13(text) {
	var converting = true;
	for (var x = 0; x < text.length; x++) {
		if (text[x] == "[" && converting) converting = false;
		if (text[x] == "]" && !converting) converting = true;
		
		if ((text[x] >= "A" && text[x] <= "M") || (text[x] >= 'a' && text[x] <= 'm')) text[x] += 13;
		if ((text[x] >= "N" && text[x] <= "Z") || (text[x] >= 'n' && text[x] <= 'z')) text[x] -= 13;
	}
	
	return text;
}

function decode(id) {
	var el = document.getElementById(id);
	if (el && el.className.indexOf("encoded") >= 0) {
		el.innerHtml = rot13(el.innerHtml);
	}
	this.parentNode.removeChild(this);
}

/*
 * The nice thing about embedding a webkit core into the application is,
 * that you don't need to bother with cross browser compatibility and
 * only stick with the one thing that actually works for webkit.
 */
window.setTimeout(function(){
	var dr = document.readyState;
	if (dr == "loaded" || dr == "complete") {
		init();
	} else {
		window.setTimeout(arguments.callee, 10);
	}
}, 10);
