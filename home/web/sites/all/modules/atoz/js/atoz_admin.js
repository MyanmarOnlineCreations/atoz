jQuery(document).ready(function() {
    // jQuery("#uploader").plupload({
    //     // General settings
    //     runtimes: 'html5,flash,silverlight,html4',
    //     url: "/sites/all/modules/atoz/applications",

    //     // Maximum file size
    //     max_file_size: '2mb',

    //     chunk_size: '1mb',

    //     // Resize images on clientside if we can
    //     resize: {
    //         width: 200,
    //         height: 200,
    //         quality: 90,
    //         crop: true // crop to exact dimensions
    //     },

    //     // Specify what files to browse for
    //     filters: [{
    //             title: "Image files",
    //             extensions: "jpg,gif,png"
    //         },
    //         {
    //             title: "Zip files",
    //             extensions: "zip,avi"
    //         }
    //     ],

    //     // Rename files by clicking on their titles
    //     rename: true,

    //     // Sort files
    //     sortable: true,

    //     // Enable ability to drag'n'drop files onto the widget (currently only HTML5 supports that)
    //     dragdrop: true,

    //     // Views to activate
    //     views: {
    //         list: true,
    //         thumbs: true, // Show thumbs
    //         active: 'thumbs'
    //     },

    //     // Flash settings
    //     flash_swf_url: '/plupload/js/Moxie.swf',

    //     // Silverlight settings
    //     silverlight_xap_url: '/plupload/js/Moxie.xap'
    // });
    jQuery ("#my-atoz-user-edit #edit-city").change (function () {
        checkDivision ();
    });


    jQuery ("#my-atoz-outlet-edit #edit-city").change (function () {
        checkDivision ();
    });

    jQuery ("#my-atoz-staff-edit #edit-city").change (function () {
        checkDivision ();
    });


    jQuery ("#my-atoz-user-filter #edit-filter-division").change (function () {
        checkFilterDivision ();
    });

    jQuery ("#my-atoz-user-edit #edit-consumer-type").change (function () {
        checkConsumerType ();
    });

    jQuery("#my-atoz-redeem-edit #edit-gift").change(function () {
        checkGift();
    })

    jQuery("#my-atoz-point-collect-edit #edit-total-amount").keyup(function () {
        calculatePoint();
    })

    
 
    checkDivision ();
    checkFilterDivision ();
    checkConsumerType ();

    function checkDivision () {

        var div=jQuery ("#edit-city").val ();

        for (var i = 1; i <= 15; i++)
        {
            if(div == i){
                jQuery(".form-item-township"+ i).show();
            }else{
                jQuery(".form-item-township"+ i).hide();
            }
        }
        
    }

    function checkFilterDivision () {
        var value=jQuery ("#edit-filter-division").val ();
        if(value == '5000'){
            jQuery(".form-item-ygn-township").show ();
            jQuery(".form-item-mdy-township").hide ();
            jQuery(".form-item-pol-township").hide ();
            jQuery(".form-item-mhl-township").hide ();
            
        }else if(value == '5001'){
            jQuery(".form-item-ygn-township").hide ();
            jQuery(".form-item-mdy-township").show ();
            jQuery(".form-item-pol-township").hide ();
            jQuery(".form-item-mhl-township").hide ();
        }else if(value == '5002'){
            jQuery(".form-item-ygn-township").hide ();
            jQuery(".form-item-mdy-township").hide ();
            jQuery(".form-item-pol-township").show ();
            jQuery(".form-item-mhl-township").hide ();
        }else if(value == '5003'){
            jQuery(".form-item-ygn-township").hide ();
            jQuery(".form-item-mdy-township").hide ();
            jQuery(".form-item-pol-township").hide ();
            jQuery(".form-item-mhl-township").show ();
        }else{
            jQuery(".form-item-ygn-township").show ();
            jQuery(".form-item-mdy-township").hide ();
            jQuery(".form-item-pol-township").hide ();
            jQuery(".form-item-mhl-township").hide ();
        }
        
    }

    function checkConsumerType () {
        var type=jQuery ("#edit-consumer-type").val ();
        if(type=="2002"){
            jQuery(".form-item-consumer-sub-type2001").hide();
            jQuery(".form-item-consumer-sub-type2002").show();
        }else{
            jQuery(".form-item-consumer-sub-type2001").show();
            jQuery(".form-item-consumer-sub-type2002").hide();
        }
        
    }
});


function upload_completed (up,files) {
    imageWidth = jQuery(window).width();

    jQuery.each(files, function () {
		var img = new mOxie.Image();
		img.onload = function () {
			jQuery('#pick-image').empty();
			this.embed(jQuery('#pick-image').get(0), {
				width: imageWidth,
				height: 100
			});
			jQuery('#pick-image').addClass("loaded");

            jQuery(".plupload_filelist_footer").hide ();
		};
		img.onembedded = function () {
			this.destroy();
		};
		img.onerror = function () {
			this.destroy();
		};
		img.load(this.getSource());
	});

}

function upload_file (up,files) {
    jQuery(".plupload_filelist_footer").show ();
}


function upload_completed2 (up,files) {
    imageWidth = jQuery(window).width();

    jQuery.each(files, function () {
		var img = new mOxie.Image();
		img.onload = function () {
			jQuery('#pick-image2').empty();
			this.embed(jQuery('#pick-image2').get(0), {
				width: imageWidth,
				height: 100
			});
			jQuery('#pick-image2').addClass("loaded");

            jQuery(".plupload_filelist_footer").hide ();
		};
		img.onembedded = function () {
			this.destroy();
		};
		img.onerror = function () {
			this.destroy();
		};
		img.load(this.getSource());
	});

}

function upload_file2 (up,files) {
    jQuery(".plupload_filelist_footer").show ();
}

function checkGift(){
    var value = jQuery('#my-atoz-redeem-edit #edit-gift').val();

    var value_arr = value.split('|');
    var gift_id = value_arr[0];
    var gpoint = value_arr[1];
    var qty = value_arr[2];

    qty = qty - 1;

    jQuery('#my-atoz-redeem-edit #redeem_gift_id').val(gift_id);
    jQuery('#my-atoz-redeem-edit #edit-redeem-point').val(gpoint);
    jQuery('#my-atoz-redeem-edit #edit-qty').val(qty);
}

function calculatePoint(){

    var total = jQuery('#my-atoz-point-collect-edit #edit-total-amount').val();

    var level = jQuery('#my-atoz-point-collect-edit #edit-member-level').val();

    var get_point = jQuery('#my-atoz-point-collect-edit #edit-get-point').val();

    var spent_amount = jQuery('#my-atoz-point-collect-edit #edit-spent-amount').val();

    var point = ( total / spent_amount ) * get_point;

    jQuery('#my-atoz-point-collect-edit #edit-point').val(point);
}



