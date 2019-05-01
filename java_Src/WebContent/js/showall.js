$(document).ready(function() {
	//显示病人信息时，光标移动到字体上时显示的颜色;红色；
	 var on_color={
     		color:"#FF1111"
     };
	 //光标移开字体时显示无色
     var out_color={
     		color:"#000000"
     };
     //动态创建的td的css
     var td_css={
		width:"10%",
		height:"30px"
 	};
    var bn_css={
		width:"20%",
		height:"60%",
		background:"#ccc",
		border:"none",
		margin:"15px"
    };
	//实现搜索功能：
	$(".search_box input").on("keyup", function(){
		var search_input=$(".search_box input");
		if (search_input.val() != ''){
			 $('[id = patientname_tr]').each(function(i){
				 	var li=$(this).children ().children ();
				 	if(li.text().indexOf(search_input.val().trim())>=0)
				 	{
				 		$(this).show();
			 		}else{
			 			$(this).hide();
			 		}
			 });
		}else{
			//当输入为空的话，全部显示
			 $('[id = patientname_tr]').each(function(i){
				 $(this).show();
			 });
		}
	});
	//实现点击图片时，在浏览器中放大查看和再次点击时，复原的功能：
	 $("body").on('click','img',function(){  
	        var _this = $(this);//将当前的img元素作为_this传入函数  
	        imgShow("#outerdiv", "#innerdiv", "#bigimg", _this);  
	 }); 
    function imgShow(outerdiv, innerdiv, bigimg, _this){  
        var src = _this.attr("src");//获取当前点击的pimg元素中的src属性  
        $(bigimg).attr("src", src);//设置#bigimg元素的src属性  
            /*获取当前点击图片的真实大小，并显示弹出层及大图*/  
        $("<img/>").attr("src", src).load(function(){  
            var windowW = $(window).width();//获取当前窗口宽度  
            var windowH = $(window).height();//获取当前窗口高度  
            var realWidth = this.width;//获取图片真实宽度  
            var realHeight = this.height;//获取图片真实高度  
            var imgWidth, imgHeight;  
            var scale = 0.8;//缩放尺寸，当图片真实宽度和高度大于窗口宽度和高度时进行缩放 
            if(realHeight>windowH*scale) {//判断图片高度  
                imgHeight = windowH*scale;//如大于窗口高度，图片高度进行缩放  
                imgWidth = imgHeight/realHeight*realWidth;//等比例缩放宽度  
                if(imgWidth>windowW*scale) {//如宽度扔大于窗口宽度  
                    imgWidth = windowW*scale;//再对宽度进行缩放  
                }  
            } else if(realWidth>windowW*scale) {//如图片高度合适，判断图片宽度  
                imgWidth = windowW*scale;//如大于窗口宽度，图片宽度进行缩放  
                imgHeight = imgWidth/realWidth*realHeight;//等比例缩放高度  
            } else {//如果图片真实高度和宽度都符合要求，高宽不变  
                imgWidth = realWidth;  
                imgHeight = realHeight;  
            }  
            $(bigimg).css("width",imgWidth);//以最终的宽度对图片缩放  
            var w = (windowW-imgWidth)/2;//计算图片与窗口左边距  
            var h = (windowH-imgHeight)/2;//计算图片与窗口上边距  
            $(innerdiv).css({"top":h, "left":w});//设置#innerdiv的top和left属性  
            $(outerdiv).fadeIn("fast");//淡入显示#outerdiv及.pimg  
        });  
        $(outerdiv).click(function(){//再次点击淡出消失弹出层  
            $(this).fadeOut("fast");  
        });  
    } 
    //获取全部病人的信息并以表格的形式显示：
	$.ajax({url:"/web/showall?show=all",success:function(result){
        var all_patientinfo=eval("("+result+")");
		var table=$("<table>");
        table.appendTo($(".left"));
        var num_patient=all_patientinfo.length;
        for(var i=0;i<num_patient;i++)
    	{
    		var tr=$("<tr id='patientname_tr'></tr>");
    		tr.appendTo(table);
    		var name=all_patientinfo[i];
    		var td=$("<td id='patientname_td'><li id='patientname_li' style='list-style-type:none'>"+name+"</li></td>");
    		td.appendTo(tr);
    	}
    	//为每个button绑定事件：
    	/*
    	 * 这儿有几个注意点：1.想要给有相同的id的组件添加事件时，jquery中必须使用each，而不能是i的方式：https://blog.csdn.net/w405722907/article/details/79147630
    	2.如果按照第一个连接的方式的话，就会出现这个事件只是添加给了第一个id组件，而不是所有的，所以，出现了第二个问题：https://blog.csdn.net/lwang_IT/article/details/83791411
    	 */
        /*
         	* 鼠标移动到li上时，字体变为红色
         	* 移出时，变为原来的颜色；
         */
       
    	$('[id = patientname_li]').each(function(i){
    		$(this).on({
    			mouseover:function(){
					$(this).css(on_color);
				},
				mouseout:function(){
					$(this).css(out_color);
				}
    		});
    		$(this).click(function(){
    			//首先清除之前显示的内容
    			$(".right").empty();
    			var patientname=$(this).text();	
    			$.post("/web/showall",
    			{
    				show:'patientName',
    				patientName:patientname
    			},
    			function(result,status){
    				if(status=="success")
    				{
    					
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
        		        	var patientId=pinfo.id;
        		        	var patientName=pinfo.patientName;
        		        	var patientSex=pinfo.patientSex;
        		        	var patientAge=pinfo.patientAge;
        		        	var patientAddress=pinfo.patientAddress;
        		        	var patientWeight=pinfo.patientWeight;
        		        	var studyDate=sinfo.studyDate;
        		        	var studyId=sinfo.id;
        		        	var studyBodyPart=sinfo.studyBodyPart;
        		        	var studyModality=seinfo.studyModality;
        		        	var seriesId=seinfo.id;
        		        	var seriesDate=seinfo.seriesDate;
        		        	var imageDate=iinfo.imageDate;
        		        	var studyDescription=sinfo.studyDescription;
        		        	var seriesDescription=seinfo.seriesDescription;
        		        	var imagePath=iinfo.imagePath;
        		        	//构造一个8行6列的table
        		        	var table=$("<table border='1px' bordercolor='#F0F8FF' cellspacing='0' cellpadding='0'>");
        			        table.appendTo($(".right"));
        		    		var tr1=$("<tr></tr>");
        		    		tr1.appendTo(table);
        		    		var td1=$("<td>姓名：</td><td><input type='text' value='' id='name"+i+"'></td>"+
        		    				"<td>性别：</td><td><input type='text' value='' id='sex"+i+"'></td>"+
        		    				"<td>年龄：</td><td><input type='text' value='' id='age"+i+"' placeholder='只能输入数字' ></td>"+
        		    				"<td class='img_td'><div class='jqzoom'><img id='img_id' src=''/></div></td>");
        		    		td1.appendTo(tr1);
        		    		var tr2=$("<tr></tr>");
        		    		tr2.appendTo(table);
        		    		var td2=$("<td>体重：</td><td><input type='text'  value='' id='weight"+i+"' placeholder='只能输入数字' ></td>"+
        		    				"<td>住址：</td><td><input type='text' value='' id='address"+i+"'></td>"+
        		    				"<td>检查部位：</td><td><input type='text' value='' id='studyBody"+i+"'></td>");
        		    		td2.appendTo(tr2);
        		    		var tr3=$("<tr></tr>");
        		    		tr3.appendTo(table);
        		    		var td3=$("<td>检查类型：</td><td><input type='text' value='' id='studyM"+i+"'></td>"+
        		    				"<td>检查日期：</td><td><input type='text' value='' id='studyDate"+i+"'></td>"+
        		    				"<td>系列日期：</td><td><input type='text' value='' id='seriesDate"+i+"'></td>" +
        		    				"<td>图片日期：</td><td><input type='text' value='' id='imageDate"+i+"'></td>");
        		    		td3.appendTo(tr3);
        		    		var tr4=$("<tr></tr>");
        		    		tr4.appendTo(table);
        		    		var td4=$("<td>检查描述：</td><td class='des_td'><textarea  value='' id='studyDes"+i+"'></textarea></td>"+
        		    				"<td>系列描述：</td><td class='des_td'><textarea value='' id='seriesDes"+i+"'></textarea></td>"+
        		    				"<td colspan='2'><button id='edit_bn'>编辑</button><button id='save_bn'>保存</button><button id='del_bn'>删除</button></td>");
        		    		td4.appendTo(tr4);
        		    		var tr5=$("<tr id='hide_tr'></tr>");
        		    		tr5.appendTo(table);
        		    		var td5=$("<td><input type='text' value="+patientId+" id='patientId"+i+"'></td>"+
        		    				"<td></td><td><input type='text' value="+studyId+" id='studyId"+i+"'></td>"+
        		    				"<td></td><td><input type='text' value="+seriesId+" id='seriesId"+i+"'></td>"+
        		    				"<td></td><td></td><td></td>");
        		    		td5.appendTo(tr5);
        		    		$(".right").append("</table>");
        		    		//添加css
        		    		$(".img_td").attr("colspan",2);
        		    		$(".img_td").attr("rowspan",2);
        		    		$(".des_td").attr("colspan",2);
        		    		
        		    		$(".right td").css(td_css);
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
        		    		$(".right input").attr("readonly","readonly");
        		    		$("textarea").width(1.6*$(".right td").width());
        		    		$("textarea").attr("readonly","readonly");
        		    		$("button").css(bn_css);
        		    	}
        		        //将最后一行隐藏：
        		        $('[id = hide_tr]').each(function(i){
        		        	$(this).hide();
        		        });
        		        /*
        		         *此处有二个纠结的bug问题：第一个就是ajax因为是异步处理，所以，关于在Ajax中使用$(this)的问题
        		         *解决之道： https://blog.csdn.net/u013661335/article/details/79338432#commentBox
        		         *第二个bug就是关于多次进行Ajax请求时，如果使用的是全局的XMLHttpRequest变量的话，会出现只接收最后一次响应的情况
        		         *此处的解决：https://bbs.csdn.net/topics/390987983?page=1
        		         *这儿可能还会存在一个问题：就是AJAX异步方式，在响应后，可能会出现乱序接收的问题。
        		        */
        		        //统一显示图片
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
    			                	here.attr("width","100%");
    			                	here.attr("height","100px");
    			                }
        		            }
        		        });
        		        
        		        //编辑按钮的事件：
        		        $('[id = edit_bn]').each(function(i){
        		        	$(this).click(function(){
        		            	$('#sex'+i+'').attr("readonly",false);
        		        		$('#age'+i+'').attr("readonly",false);
        		        		//只能输入数字
        		        		$('#age'+i+'').on('input propertychange', function(e) {
        		        			var text = $(this).val().replace(/[^\d]/g, "");
        		        			$(this).val(text);	
        		        		});
        		        		$('#address'+i+'').attr("readonly",false);
        		        		$('#weight'+i+'').attr("readonly",false);
        		        		$('#weight'+i+'').on('input propertychange', function(e) {
        		        			var text = $(this).val().replace(/[^\d]/g, "");
        		        			$(this).val(text);	
        		        		});
        		        		$('#studyDes'+i+'').attr("readonly",false);
        		        		$('#seriesDes'+i+'').attr("readonly",false);
        		        		$('#studyDate'+i+'').attr("readonly",false);
        		        		$('#seriesDate'+i+'').attr("readonly",false);
        		        		$('#imageDate'+i+'').attr("readonly",false);
        		        		$('#studyBody'+i+'').attr("readonly",false);
        		        	});
        		        });
        		        //删除按钮事件：
        		        $('[id=del_bn]').each(function(i){
        		        	$(this).click(function(){
        		        		var patientId=$('#patientId'+i+'').val().trim();
        		        		var here=$(this);
        		        		$.get("/web/showall?show=del&patientid="+patientId,function(data,status){
        		        			if(status=="success")
        		        			{
        		        				if(data=="删除成功!")
        		        				{
        		        					here.parent().parent().parent().hide();
        		        					alert(data);
        		        				}else{
        		        					alert(data);
        		        				}
        		        			}
        		        		});
        		        	});
        		        });
        		        //保存按钮的事件：
        		        $('[id = save_bn]').each(function(i){
        		        	$(this).click(function(){
        		            	var new_sex=$('#sex'+i+'').val().trim();
        		            	var new_age=$('#age'+i+'').val().trim();
        		        		var new_addr=$('#address'+i+'').val().trim();
        		        		var new_weight=$('#weight'+i+'').val().trim();
        		        		var new_studyDes=$('#studyDes'+i+'').val().trim();
        		        		var new_seriesDes=$('#seriesDes'+i+'').val().trim();
        		        		var new_studyDate=$('#studyDate'+i+'').val().trim();
        		        		var new_seriesDate=$('#seriesDate'+i+'').val().trim();
        		        		var new_imageDate=$('#imageDate'+i+'').val().trim();
        		        		var new_studyBody=$('#studyBody'+i+'').val().trim();
        		        		var new_patientId=$('#patientId'+i+'').val().trim();
        		        		var new_studyId=$('#studyId'+i+'').val().trim();
        		        		var new_seriesId=$('#seriesId'+i+'').val().trim();
        		        		console.log("new_studyDes:");
        		        		console.log(new_studyDes);
        		        		console.log("pid:",new_patientId);
        		        		console.log("sid:",new_studyId);
        		        		console.log("seid:",new_seriesId);
        		        		//ajax post请求：
        		        		 $.post("/web/showall",
    	        				  {
        		        			 show:'save_new',
        		        			 new_sex:new_sex,
        		        			 new_age:new_age,
        		        			 new_addr:new_addr,
        		        			 new_weight:new_weight,
        		        			 new_studyDes:new_studyDes,
        		        			 new_seriesDes:new_seriesDes,
        		        			 new_studyDate:new_studyDate,
        		        			 new_seriesDate:new_seriesDate,
        		        			 new_imageDate:new_imageDate,
        		        			 new_studyBody:new_studyBody,
        		        			 new_patientId:new_patientId,
        		        			 new_studyId:new_studyId,
        		        			 new_seriesId:new_seriesId
    	        				  },
    	        				  function(result,status){
	    	        			      if(status=="success")
	    	        				  {
	    	        			    	  alert(result);
	    	        			    	  $(".right input").attr("readonly","readonly");
	    	        			    	  $("textarea").attr("readonly","readonly");
	    	        				  }
    	        				  });
        		        		});
        		        	});
    			      }});
    	         });
	       });
      }});
});
