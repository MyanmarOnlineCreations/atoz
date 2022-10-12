<?php 

	function myatoz_redemption_report($parameter1=null,$parameter2=null){
		$output = '';

		if($parameter1 == 'csv'){
			$output .= myatoz_redemption_report_download();
		}else{
			$contentForm = drupal_get_form('myatoz_redemption_report_filter_form');
			$output .= drupal_render($contentForm);
			$output .= myatoz_redemption_report_list();
		}
		
		return $output;
	}

	function myatoz_redemption_report_list(){

		global $user;

		$output="";	
		$link = $_SERVER["REQUEST_URI"];
		$rowsPerPage = 100;
		$dataPerRow = 1;
		$offset = atoz_admin_page_offset($rowsPerPage);		
    	$data = array();

		$sortOptions = array (
			"default"			=> "ur.created_date",
			"ur.gmid"			=> "desc",
			"ur.created_date"  	=> "desc",
			"member_level"		=> "desc",
			"ur.redemption_point"	=> "desc",			
			"outlet"			=> "desc",
			"gift_name"			=> "desc",
		);

		$styleArray = array (
			"",
			"",
			"",
			"right",
			"",
			"",	
		);

		$headerFields = array (			
	        t("Date") 			=> "ur.created_date",
	        t("Member ID")     	=> "ur.gmid",
	        t("Gift Name")		=> "gift_name",
	        t("Redeemed Point")	=> "ur.redemption_point",
	        t("Outlet")			=> "outlet",
	        t("Member Tier")   	=> "member_level",
		);

		$query = db_select('tbl_home_users_redeems','ur');
		$query ->leftjoin('tbl_home_members_info','m','ur.gmid = m.gmid');
		$query ->leftjoin('tbl_home_gift','g','ur.grid = g.grid');
		$query ->leftjoin('tbl_home_users_info','i','ur.created_by = i.uid');
		$query ->fields('i',array('outlet'));
		$query ->fields('g');
		$query ->fields('ur',array('gmid','redemption_point','created_date','created_by'));
		$query ->fields('m',array('member_level'));
		
		if(isset($_SESSION[ATOZ_REDEEM_MEM_ID_FILTER])){
			$query->condition('ur.gmid', $_SESSION[ATOZ_REDEEM_MEM_ID_FILTER]);
		}

		if(isset($_SESSION[ATOZ_REDEEM_MEMBER_LEVEL_FILTER])){
			$query->condition('m.member_level',$_SESSION[ATOZ_REDEEM_MEMBER_LEVEL_FILTER]);
		}

		if(isset($_SESSION[ATOZ_REDEEM_DATE_FILTER])){
			$query->where(" date(ur.created_date) = '" . $_SESSION[ATOZ_REDEEM_DATE_FILTER] ."'" );
		}

		if(isset($_SESSION[ATOZ_REDEEM_OUTLET_FILTER])){
			$query->condition('i.outlet',$_SESSION[ATOZ_REDEEM_OUTLET_FILTER]);
		}

		if(isset($_SESSION[ATOZ_REDEEM_MEMBER_TYPE_FILTER])){
			$query->condition('member_type',$_SESSION[ATOZ_REDEEM_MEMBER_TYPE_FILTER]);
		}
		$query->range($offset, $rowsPerPage);

	    atoz_checksort ($sortOptions, $query);

		$result = $query->execute();

		$outlet_arr = atoz_get_outlet();

		$member_level_arr = atoz_member_level();

		

		if($result){

			foreach($result as $row){

				if($row->outlet == '' || $row->outlet == null){
					$outlet = '';
				}else{
					$outlet = $outlet_arr[$row->outlet];
				}

				$list['date'] = date('d-M-y',strtotime($row->created_date));
				$list['gmid'] = $row->gmid;
				$list['gift_name'] = $row->gift_name;
				$list['redemption_point'] = number_format($row->redemption_point);
				$list['outlet'] = $outlet;				
				$list['member_level'] = $member_level_arr[$row->member_level];

				$data[] = $list;
			}
		}	

		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$pager = atoz_admin_pager_by_query($pageQuery,"ur", "gmid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		$output .= '<h2 class="list_header"> '.t("Redemption Report").' <span class="myst-admin-title-total">('.$total.')</span></h2>';

		$output .= '<a class="downlod-csv" href = "/atoz/redemptionreport/csv">'.t('Download CSV').'</a>';


		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;
	}


	function myatoz_redemption_report_filter_form($form, &$form_state){

		if (isset ($_SESSION[ATOZ_REDEEM_DATE_FILTER])) {
	        $defaultDateFilter = $_SESSION[ATOZ_REDEEM_DATE_FILTER];
	    }
	    else {
	        $defaultDateFilter = "";
	    } 	

	    if (isset ($_SESSION[ATOZ_REDEEM_MEMBER_LEVEL_FILTER])) {
	        $defaultMemberLevelFilter = $_SESSION[ATOZ_REDEEM_MEMBER_LEVEL_FILTER];
	    }
	    else {
	        $defaultMemberLevelFilter = "";
	    } 	

	    if (isset($_SESSION[ATOZ_REDEEM_MEM_ID_FILTER])) {
	        $defaultMIdFilter = $_SESSION[ATOZ_REDEEM_MEM_ID_FILTER];
	    }
	    else {
	        $defaultMIdFilter = "";
	    } 

	    if (isset ($_SESSION['ATOZ_REDEEM_OUTLET_FILTER'])) {
	        $defaultOutletFilter = $_SESSION['ATOZ_REDEEM_OUTLET_FILTER'];
	    }
	    else {
	        $defaultOutletFilter = "";
	    } 	

	    if (isset ($_SESSION[ATOZ_REDEEM_MEMBER_TYPE_FILTER])) {
	        $defaultMemberTypeFilter = $_SESSION[ATOZ_REDEEM_MEMBER_TYPE_FILTER];
	    }
	    else {
	        $defaultMemberTypeFilter = "";
	    } 	    

		$form = array('#id' => 'my-atoz-redeem-report-filter-form');
	

		$form['filter_date'] = array (
		    '#title' => t('Date'),
	        '#type' => 'date_popup',
	        '#date_format' => 'Y-m-d',
	        '#default_value' => $defaultDateFilter,
	    );
	    
	    $form['member_id'] = array (
		    '#title' => t('Member ID'),
	        '#type' => 'textfield',
	        '#default_value' => $defaultMIdFilter,
	    );

	    $member_level_arr = atoz_member_level();

	    $form['member_level'] = array (
		    '#title' => t('Member Tier'),
	        '#type' => 'select',
	        '#options' => $member_level_arr,
	        '#default_value' => $defaultMemberLevelFilter,
	    );

	    $outlet_arr = atoz_get_outlet();
	    $outlet_arr[""] = t('-- Select --'); 

	    $form['outlet'] = array (
		    '#title' => t('Outlet'),
	        '#type' => 'select',
	        '#options' => $outlet_arr,
	        '#default_value' => $defaultOutletFilter,
	    );

	    $member_type_arr = atoz_member_types();

	    $form['member_type'] = array (
		    '#title' => t('Member Type'),
	        '#type' => 'select',
	        '#options' => $member_type_arr,
	        '#default_value' => $defaultMemberTypeFilter,
	    );

	    $form['submit'] = array (
	        '#type' => 'submit',
	        '#weight' => 98,
	        '#value' => t('Search')
	    );

	    $form['reset'] = array (
	        '#type' => 'submit',
	        '#weight' => 99,
	        '#value' => t('Reset')
	    );

		return $form;
	}

	function myatoz_redemption_report_filter_form_validate($form, &$form_state){

	}

	function myatoz_redemption_report_filter_form_submit($form, &$form_state){

		if ($form_state["values"]["op"] == t("Reset")) {

			unset($_SESSION[ATOZ_REDEEM_DATE_FILTER]);
			unset($_SESSION[ATOZ_REDEEM_MEMBER_LEVEL_FILTER]);
			unset($_SESSION[ATOZ_REDEEM_MEM_ID_FILTER]);
			unset($_SESSION[ATOZ_REDEEM_OUTLET_FILTER]);
			unset($_SESSION[ATOZ_REDEEM_MEMBER_TYPE_FILTER]);
			
		}else{
			
			$dateFilter = $form_state["values"]["filter_date"];
			$memberLevelFilter = $form_state["values"]["member_level"];
			$memberIdFilter = $form_state["values"]["member_id"];
			$outletFilter = $form_state["values"]["outlet"];
			$memberTypeFilter = $form_state["values"]["member_type"];
			
			if (!$dateFilter) {
				unset($_SESSION[ATOZ_REDEEM_DATE_FILTER]);
			}
			else {
				$_SESSION[ATOZ_REDEEM_DATE_FILTER] = $dateFilter;
			}	

			if (!$memberLevelFilter) {
				unset($_SESSION[ATOZ_REDEEM_MEMBER_LEVEL_FILTER]);
			}
			else {
				$_SESSION[ATOZ_REDEEM_MEMBER_LEVEL_FILTER] = $memberLevelFilter;
			}

			if (!$memberIdFilter) {
				unset($_SESSION[ATOZ_REDEEM_MEM_ID_FILTER]);
			}
			else {
				$_SESSION[ATOZ_REDEEM_MEM_ID_FILTER] = $memberIdFilter;
			}	

			if (!$outletFilter) {
				unset($_SESSION[ATOZ_REDEEM_OUTLET_FILTER]);
			}
			else {
				$_SESSION[ATOZ_REDEEM_OUTLET_FILTER] = $outletFilter;
			}

			if (!$memberTypeFilter) {
				unset($_SESSION[ATOZ_REDEEM_MEMBER_TYPE_FILTER]);
			}
			else {
				$_SESSION[ATOZ_REDEEM_MEMBER_TYPE_FILTER] = $memberTypeFilter;
			}
			
		}

	}

	function myatoz_redemption_report_download(){

		set_time_limit (0);

	    $header = array ("Date","Member ID","Gift Name","Point","Outlet","Member Tier");

	    header('Content-type: application/csv');
	    header ("Content-Disposition: attachment; filename=redemption_report.csv");
	    echo implode (",", $header) . "\n";

	    $query = db_select('tbl_home_users_redeems','ur');
		$query ->leftjoin('tbl_home_members_info','m','ur.gmid = m.gmid');
		$query ->leftjoin('tbl_home_gift','g','ur.grid = g.grid');
		$query ->leftjoin('tbl_home_users_info','i','ur.created_by = i.uid');
		$query ->fields('i',array('outlet'));
		$query ->fields('g');
		$query ->fields('ur',array('gmid','created_date','created_by'));
		$query ->fields('m',array('member_level'));
		$query ->orderBy('ur.created_date','desc');

		$result = $query->execute();

		$outlet_arr = atoz_get_outlet();

		$member_level_arr = atoz_member_level();

	    foreach ($result as $row) {

	        $point = array();
	        $point[] = myst_clean_for_csv(date('d-M-y',strtotime($row->created_date)));
			$point[] = myst_clean_for_csv($row->gmid);
			$point[] = myst_clean_for_csv($row->gift_name);
			$point[] = myst_clean_for_csv($row->gpoint);
			$point[] = myst_clean_for_csv($outlet_arr[$row->outlet]);
			$point[] = myst_clean_for_csv($member_level_arr[$row->member_level]);

	        echo implode (",", $point) . "\n";
	    }
	    exit(0);
	}

?>