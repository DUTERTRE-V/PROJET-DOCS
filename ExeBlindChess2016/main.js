//setTimeout(function(){alert("Hello")},'1000');
//setTimeout('window.parent.location.reload()','1000');

function refresh() {
        var unique = new Date();
        document.images.monEchiquier.src = "echiquier.jpg&unique=" + unique.getTime(); // ou via getElementById ça revient au même :o
}
window.onload = function (){
        setInterval("refresh()", 500);
}

