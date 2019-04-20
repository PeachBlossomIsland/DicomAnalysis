$(document).ready(function() {
	$.ajax({url:"/web/showall?show=all",success:function(result){
        var all_patientinfo=eval("("+result+")");
		var table=$("<table>");
        table.appendTo($(".left"));
        var num_patient=all_patientinfo.length;
        for(var i=0;i<num_patient;i++)
    	{
    		var tr=$("<tr></tr>");
    		tr.appendTo(table);
    		var name=all_patientinfo[i];
    		var td=$("<td id='patientname_td'><button id='patientname_button' name='"+name+"'>"+name+"</button></td>");
    		td.appendTo(tr);
    	}
    	$(".left").append("</table>");
    	//为每个button绑定事件：
    	/*
    	 * 这儿有几个注意点：1.想要给有相同的id的组件添加事件时，jquery中必须使用each，而不能是i的方式：https://blog.csdn.net/w405722907/article/details/79147630
    	2.如果按照第一个连接的方式的话，就会出现这个事件只是添加给了第一个id组件，而不是所有的，所以，出现了第二个问题：https://blog.csdn.net/lwang_IT/article/details/83791411
    	 */
    	$('[id = patientname_button]').each(function(i){
    		$(this).click(function(){
    			//首先清除之前的内容
    			$(".right").empty();
    			var patientname=$(this).attr("name");	
    			alert(patientname);
    			$.ajax({url:"/web/showall?show="+patientname,success:function(result){
    				var allinfo=$.parseJSON(result);
    		        var num=allinfo.length;
    		        //保存图像的位置
    		        var path=new Array();
    		        for(var i=0;i<num;i++)
    		    	{
    		        	//获取所有详细信息：
    		        	var iinfo=allinfo[i].iinfo;
    		        	var pinfo=allinfo[i].pinfo;
    		        	var seinfo=allinfo[i].seinfo;
    		        	var sinfo=allinfo[i].sinfo;
    		        	var patientName=pinfo.patientName;
    		        	var patientSex=pinfo.patientSex;
    		        	var patientAge=pinfo.patientAge;
    		        	var patientAddress=pinfo.patientAddress;
    		        	var patientWeight=pinfo.patientWeight;
    		        	var studyDate=sinfo.studyDate;
    		        	var studyBodyPart=sinfo.studyBodyPart;
    		        	var studyModality=seinfo.studyModality;
    		        	var seriesDate=seinfo.seriesDate;
    		        	var imageDate=iinfo.imageDate;
    		        	var studyDescription=sinfo.studyDescription;
    		        	var seriesDescription=seinfo.seriesDescription;
    		        	var imagePath=iinfo.imagePath;
    		        	//输出检查
    		        	console.log("studyDate:"+studyDate);
    		        	console.log("seriesDate:"+seriesDate);
    		        	console.log("imagePath:"+imagePath);
    		        	//构造一个8行6列的table
    		        	var table=$("<table>");
    			        table.appendTo($(".right"));
    		    		var tr1=$("<tr></tr>");
    		    		tr1.appendTo(table);
    		    		var td1=$("<td>姓名：</td><td><input type='text' value='' id='name"+i+"'></td><td class='img_td'><img id='img_id' src=''/></td>");
    		    		td1.appendTo(tr1);
    		    		var tr2=$("<tr></tr>");
    		    		tr2.appendTo(table);
    		    		var td2=$("<td>性别：</td><td><input type='text' value='' id='sex"+i+"'></td>");
    		    		td2.appendTo(tr2);
    		    		var tr3=$("<tr></tr>");
    		    		tr3.appendTo(table);
    		    		var td3=$("<td>年龄：</td><td><input type='text' value='' id='age"+i+"'></td>");
    		    		td3.appendTo(tr3);
    		    		var tr4=$("<tr></tr>");
    		    		tr4.appendTo(table);
    		    		var td4=$("<td>住址：</td><td><input type='text' value='' id='address"+i+"'></td>");
    		    		td4.appendTo(tr4);
    		    		var tr5=$("<tr></tr>");
    		    		tr5.appendTo(table);
    		    		var td5=$("<td>体重：</td><td><input type='text' value='' id='weight"+i+"'></td><td>检查日期：</td><td><input type='text' value='' id='studyDate"+i+"'></td><td>检查部位：</td><td><input type='text' value='' id='studyBody"+i+"'></td>");
    		    		td5.appendTo(tr5);
    		    		var tr6=$("<tr></tr>");
    		    		tr6.appendTo(table);
    		    		var td6=$("<td>检查类型：</td><td><input type='text' value='' id='studyM"+i+"'></td><td>系列日期：</td><td><input type='text' value='' id='seriesDate"+i+"'></td><td>图片日期：</td><td><input type='text' value='' id='imageDate"+i+"'></td>");
    		    		td6.appendTo(tr6);
    		    		var tr7=$("<tr></tr>");
    		    		tr7.appendTo(table);
    		    		var td7=$("<td>检查描述：</td><td class='des_td'><input type='text' value='' id='studyDes"+i+"'></td>");
    		    		td7.appendTo(tr7);
    		    		var tr8=$("<tr></tr>");
    		    		tr8.appendTo(table);
    		    		var td8=$("<td>系列描述：</td><td class='des_td'><input type='text' value='' id='seriesDes"+i+"'></td>");
    		    		td8.appendTo(tr7);
    		    		$(".right").append("</table>");
    		    		//添加css
    		    		var img_css={
    		    			colspan:"4",
    		    	 		rowspan:"4"
    		    		};
    		    		$(".img_td").css(img_css);
    		    		var des_css={
    		    			colspan:"5"	
    		    		};
    		    		$(".des_td").css(des_css);
//    		    		var img_css={
//    		    				width:"200px",
//    		    				height:"200px"
//    		    		};
//    		    		$("#img_td").css(img_css);
    		    		//给table中的字段赋值
    		    		
    		    		$('#name'+i+'').val(patientName);
    		    		$('#sex'+i+'').val(patientSex);
    		    		$('#age'+i+'').val(patientAge);
    		    		$('#address'+i+'').val(patientAddress);
    		    		$('#weight'+i+'').val(patientWeight);
    		    		$('#studyM'+i+'').val(studyModality);
    		    		$('#studyDes'+i+'').val(studyDescription);
    		    		$('#seriesDes'+i+'').val(seriesDescription);
    		    		$('#studyDate'+i+'').val(studyDate);
    		    		$('#seriesDate'+i+'').val(seriesDate);
    		    		$('#imageDate'+i+'').val(imageDate);
    		    		$('#studyBody'+i+'').val(studyBodyPart);
    		    		path[i]=imagePath;
    		    	}
    		        /*
    		         *此处有二个纠结的bug问题：第一个就是ajax因为是异步处理，所以，关于在Ajax中使用$(this)的问题
    		         *解决之道： https://blog.csdn.net/u013661335/article/details/79338432#commentBox
    		         *第二个bug就是关于多次进行Ajax请求时，如果使用的是全局的XMLHttpRequest变量的话，会出现只接收最后一次响应的情况
    		         *此处的解决：https://bbs.csdn.net/topics/390987983?page=1
    		         *这儿可能还会存在一个问题：就是AJAX异步方式，在响应后，可能会出现乱序接收的问题。
    		         */
    		        $('[id = img_id]').each(function(i){
    		        	console.log("i:"+i);
    		        	var imagePath=path[i];
    		        	console.log("imagePath:"+imagePath);
    		        	var xmlHttp=new XMLHttpRequest();
    		            xmlHttp.open("GET","/web/getImg?path="+imagePath,true);
    		            xmlHttp.send(null);
    		            var here=$(this);
    		            xmlHttp.onreadystatechange=function () {
			                if (xmlHttp.readyState ==4 && xmlHttp.status ==200){
			                	console.log("AJAX:"+xmlHttp.status);
			                	var imageType = xmlHttp.getResponseHeader("Content-Type");
			                	console.log(xmlHttp.response);
			                	here.attr("src","data:"+imageType+";base64,"+xmlHttp.response);
			                }
    		            }
    		        });
    			}});
    		});
    	});
	}});
});
