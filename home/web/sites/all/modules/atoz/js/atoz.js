var gifTooLarge = "";
var fileUploadFailed = "";
var maxPost = 0;
var ratingNumber = 0;


jQuery(document).ready(function() {
    var checkTop = false;

    if (window != top && !checkTop) {
        checkTop = true;

        try {
            targetLocation = window.location;

            result = parent.currentDestination(targetLocation.toString());
        }
        catch (err) {
            alert (err);
           //top.location = window.location;
        }
    }

    var destinationUrl = "";

    // go to previous page
	jQuery(".my-atoz-back").click (function () {
	    window.history.back();
	});

    jQuery(".logo").click (function () {
        url = "/webapp/home";

        window.location = url;
    });

    jQuery(".my-atoz-close").click (function () {
        jQuery(".my-atoz-overlay").animate ({
            opacity: 0
        }, function () {
            jQuery(".my-atoz-overlay").hide ();
            jQuery(".my-atoz-overlay").css ("opacity", "1.0");
        });
    });


    function showErrorBlock () {
        if (jQuery("#my-atoz-error-block").length) {
            windowHeight = jQuery(window).height();
            windowWidth = jQuery(window).width();

            if (jQuery(".my-atoz-error-ok").length) {
                offset = jQuery(".my-atoz-error-ok").offset().top - jQuery(".my-atoz-error-message").offset().top;

                contentHeight = jQuery(".my-atoz-error-ok").height () + offset;
            }
            else {
                contentHeight = jQuery(".my-atoz-error-message").height ();
    ;
            }

            marginTop = (windowHeight - contentHeight) / 3;

/*
            console.log (windowHeight);
            console.log (contentHeight);
            console.log (marginTop);
            */


            jQuery(".my-atoz-error-message").css ("margin-top", marginTop + "px");
        }

         jQuery("html, body").animate({ scrollTop: 0 }, "fast");
    }

    if (jQuery("#my-atoz-error-block").is(":visible")) {
        showErrorBlock ();
    }

    jQuery(".my-atoz-error-ok").click (function () {
        jQuery("#my-atoz-error-block").animate ({
            opacity: 0
        }, function () {
            jQuery("#my-atoz-error-block").hide ();
            jQuery("#my-atoz-error-block").css ("opacity", "1.0");
        });
    });

    jQuery(".my-atoz-error-cancel").click (function () {
        jQuery("#my-atoz-error-block").animate ({
            opacity: 0
        }, function () {
            jQuery("#my-atoz-error-block").hide ();
            jQuery("#my-atoz-error-block").css ("opacity", "1.0");
        });
    });

    jQuery(".my-atoz-error-login").click (function () {
        jQuery("#my-atoz-error-block").hide ();

        window.location = "/webapp/user?destination=" + destinationUrl;
    });

    jQuery(".my-atoz-error-signup").click (function () {
        jQuery("#my-atoz-error-block").hide ();

        window.location = "/webapp/user?destination=" + destinationUrl;
    });


    /* - - - - -- - - - - - start for atoz rating giving for each atoz - - - - - -- - - */




    $(".rating input").click(function(){
       //alert("Hate it");
       var rate = $(this).val();
       ratingNumber = rate
       if(rate){
           $(".rate_submit").css("display" , "block");
       }
       else{
           $(".rate_submit").css("display" , "none");
       }

    });

    /* Click Submit Button*/
    $(".submit-btn").click(function(){
        
        console.log(ratingNumber);
        var atoz_id = $(this).attr('id');

        if(atoz_id){
            jQuery.ajax({
                url: "/webapp/ajax",
                type: "GET",
                data: {
                    atoz_id: atoz_id ,
                    rating_number : ratingNumber
                },
                success:function(data){
                   if(data == "inserted"){
                      window.location.reload();
                   }
                   else{
                      window.location.reload();
                   }

                  
                },
                error:function(data){
                    alert("fail");
                }
            });
        }

    });



    //  if don't have user account need to login 

    $(".no_login label").click(function(){
          window.location = "/webapp/signup";
    });


    

    /* - - - - - -- - - - - end for atoz rating giving for each atoz - - - - -- - - - */


    function checkWindow () {
        width = jQuery(window).width();
        height = jQuery(window).height();

        if ((height * 0.6) < width) {
            jQuery(".my-atoz-bottom").css("position", "relative");
        }
        else {
            jQuery(".my-atoz-bottom").css("position", "absolute");
        }

        if (0.05* width > 20) {
            jQuery("body").css ("font-size", "20px");
        }

        offset = (width - jQuery("#webapp-wrapper").width()) / 2;

        jQuery ("#webapp-wrapper #my-atoz-menu-block").css ("right", offset + "px");
    }

    jQuery("#webapp-wrapper .my-atoz-menu").click (function () {
        jQuery("#webapp-wrapper .my-atoz-menu-overlay").show ();
        jQuery("#webapp-wrapper .my-atoz-menu-inner-block").animate({"left":"0"}, "slow");
    });

    jQuery("#webapp-wrapper .my-atoz-menu-overlay").click (function () {
        jQuery("#webapp-wrapper .my-atoz-menu-inner-block").animate({"left":"-100%"}, "slow", function () {
            jQuery("#webapp-wrapper .my-atoz-menu-overlay").hide ();
        });
    });

    checkWindow ();

    jQuery (window).resize (checkWindow);

});


// Test for the ugliness.
if (window.location.hash == '#_=_'){

    // Check if the browser supports history.regaceState.
    if (history.replaceState) {

        // Keep the exact URL up to the hash.
        var cleanHref = window.location.href.split('#')[0];

        // Replace the URL in the address bar without messing with the back button.
        history.replaceState(null, null, cleanHref);

    } else {

        // Well, you're on an old browser, we can get rid of the _=_ but not the #.
        window.location.hash = '';

    }

}
