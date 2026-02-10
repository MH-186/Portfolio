		var content1 = document.getElementById("content1");
        var content2 = document.getElementById("content2");
        var btn1 = document.getElementById("btn1");
        var btn2 = document.getElementById("btn2");

        function openREPORT() {
            content1.style.transform = "translateX(0)";
            content2.style.transform = "translateX(2000px)";
            btn1.style.color = "#ff4b3d";
            btn2.style.color = "#000";
        }
        function openAPPO() {
            content1.style.transform = "translateX(2000px)";
            content2.style.transform = "translateX(0)";
            btn1.style.color = "#000";
            btn2.style.color = "#ff4b3d";
        }
        function openBOOK() {
            content1.style.transform = "translateX(2000px)";
            content2.style.transform = "translateX(2000px)";
            btn1.style.color = "#000";
            btn2.style.color = "#000";
        }