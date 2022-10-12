<?php

	function myatoz_member_report($parameter1=null,$parameter2=null){
		$output = '';
		if($parameter1 == 'csv'){
			$output .= myatoz_member_report_download();
		}else{
			$contentForm = drupal_get_form('myatoz_member_report_filter_form');
			$output .= drupal_render($contentForm);
			$output .= myatoz_member_report_list();
		}
		
		return $output;
	}

	function myatoz_member_report_list(){

		global $user;

		$output="";			
		$link = $_SERVER["REQUEST_URI"];
		$rowsPerPage = 100;
		$dataPerRow = 1;
		$offset = atoz_admin_page_offset($rowsPerPage);
    	$data = array();
    	$frequecny_data = array();
    	$used_data = array();

		$sortOptions = array (
			"default"			=> "",
			"name"				=> "",
			"gmid"  			=> "",
			"member_level"		=> "",
			"collected_point"	=> "",			
			"used_point"		=> "",			
			"current_point"		=> "",
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
	        t("Member ID") 				=> "gmid",
	        t("Name")      				=> "name",
	        t("Date of Birth")			=> "dob",
	        t("NRC")					=> "nrc",
	        t("Phone")					=> "phone",
	        t("Total Collected Point")	=> "collected_point",	        
	        t("Membership Type")		=> "member_type",
	        t("Member Tier")  			=> "member_level",
	        t("City")					=> "city",
	        t("Township")				=> "township",	 
	        t("Total Redeem Point")		=> "used_point",
	        t("Available Point")		=> "current_point",
	        t("Redemption Frequency")  	=> "frequnecy",
	        t("Created By")				=> "created_by",
	        t("Created Date")			=> "created_date",	        
		);


		$collect_point_query = "select i.umid, i.gmid,i.name,i.dob,i.nrc,i.phone,current_point,i.member_type,i.city,i.township,i.created_date,i.created_by,i.member_level,sum(p.point) collected_point from tbl_home_members_info i left join tbl_home_points p on i.gmid = p.gmid where 1=1 ";

		if(isset($_SESSION['ATOZ_MEMBER_LEVEL_FILTER'])){
			$collect_point_query .= " AND i.member_level = '". $_SESSION['ATOZ_MEMBER_LEVEL_FILTER'] ."'";
		}

		if(isset($_SESSION['ATOZ_MEMBER_ID_FILTER'])){
			$collect_point_query .= " AND i.gmid = '". $_SESSION['ATOZ_MEMBER_ID_FILTER'] ."'" ;
		}

		$collect_point_query .= " group by i.gmid";

		$collect_result = db_query($collect_point_query);


		$used_point_query = "select i.umid,sum(g.gpoint) used_point from tbl_home_members_info i left join tbl_home_users_redeems ur on i.gmid = ur.gmid  left join tbl_home_gift g on ur.grid = g.grid  left join tbl_home_points p on p.gmid = i.gmid group by i.gmid";

		$used_result = db_query($used_point_query);

		$redemption_frequency_query = "select m.umid,count(ur.gmid) frequnecy from tbl_home_users_redeems ur left join tbl_home_members_info m on ur.gmid = m.gmid group by ur.gmid";

		$redemption_frequency_result = db_query($redemption_frequency_query);

		if($used_result){
			foreach($used_result as $row){
				$used_data[$row->umid] = $row->used_point;
			}
		}

		if($redemption_frequency_result){
			foreach($redemption_frequency_result as $row){
				$frequency_data[$row->umid] = $row->frequnecy;
			}
		}

		$member_level_arr = atoz_member_level();

		$township_arr = atoz_townships();

		$city_arr = atoz_division();

		if($collect_result){
			foreach($collect_result as $row){

				if($row->member_level == ''){
					$member_level = '';
				}else{
					$member_level = $member_level_arr[$row->member_level];
				}

				if(isset($frequency_data[$row->umid])){
					$frequency = $frequency_data[$row->umid];
				}else{
					$frequency = 0;
				}

		// Name/DOB/NRC/Phone/Current Point/Membership Type/Member  Tier/City/Township/Address/Created By/Created Date

				// $list = array('gmid'=>'','name'=>'','member_level'=>'','collected_point'=>0,'used_point'=>0);

				$list['gmid'] = $row->gmid;
				$list['name'] = $row->name;
				$list['dob'] = $row->dob;
				$list['nrc'] = $row->nrc;
				$list['phone'] = $row->phone;
				$list['collected_point'] = $row->collected_point;
				$list['member_type'] = $row->member_type;
				$list['member_level'] = $member_level;
				$list['city']	= $city_arr[$row->city];
				$list['township'] = $township_arr[$row->township];
				$list['used_point'] = $used_data[$row->umid];
				$list['current_point'] = $row->current_point;
				$list['frequnecy'] = $frequency;
				$list['created_by']	 = $row->created_by;
				$list['created_date'] = date('Y-m-d',strtotime($row->created_date));

				$data[$row->umid] = $list;			

			}
		}

		if(isset($_SESSION['ATOZ_REDEMPTION_FREQUENCY_FILTER'])){					
			$data = array_filter($data,"frequnecy_filter");

			$total = count($data);
		}

		$query = db_select('tbl_home_members_info','m');
		$query ->fields('m',array('gmid'));

		if(isset($_SESSION['ATOZ_MEMBER_ID_FILTER'])){
			$query ->condition('member_level',$_SESSION['ATOZ_MEMBER_ID_FILTER']);
		}

		if(isset($_SESSION['ATOZ_MEMBER_LEVEL_FILTER'])){
			$query ->condition('member_level',$_SESSION['ATOZ_MEMBER_LEVEL_FILTER']);
		}	


		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$pager = atoz_admin_pager_by_query($pageQuery,"m", "gmid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);


		$output .= '<h2 class="list_header"> '.t('Members').' <span class="myst-admin-title-total">('.$total.')</span></h2>';

		$output .= '<a class="downlod-csv" href="/atoz/memberreport/csv">'.t('Download CSV').'</a>';

		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;
	}

	function myatoz_member_report_filter_form($form, &$form_state){

		if (isset ($_SESSION[ATOZ_MEMBER_ID_FILTER])) {
	        $defaultUserIDFilter = $_SESSION[ATOZ_MEMBER_ID_FILTER];
	    }
	    else {
	        $defaultUserIDFilter = "";
	    } 	

	    if (isset ($_SESSION[ATOZ_MEMBER_LEVEL_FILTER])) {
	        $defaultMemberLevelFilter = $_SESSION[ATOZ_MEMBER_LEVEL_FILTER];
	    }
	    else {
	        $defaultMemberLevelFilter = "";
	    } 	

	    if (isset ($_SESSION[ATOZ_REDEMPTION_FREQUENCY_FILTER])) {
	        $defaultRedemptionFreqFilter = $_SESSION[ATOZ_REDEMPTION_FREQUENCY_FILTER];
	    }
	    else {
	        $defaultRedemptionFreqFilter = "";
	    } 	    

		$form = array('#id' => 'my-atoz-member-report-filter-form');
	

		$form['member_id'] = array (
		    '#title' => t('Member ID'),
	        '#type' => 'textfield',
	        '#default_value' => $defaultUserIDFilter,
	    );

	    $member_level_arr = atoz_member_level();

	    $form['member_level'] = array (
		    '#title' => t('Member Tier'),
	        '#type' => 'select',
	        '#options' => $member_level_arr,
	        '#default_value' => $defaultMemberLevelFilter,
	    );

	    $form['frequnecy'] = array (
		    '#title' => t('Frequency'),
	        '#type' => 'textfield',
	        '#default_value' => $defaultRedemptionFreqFilter,
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

	function myatoz_member_report_filter_form_validate($form, &$form_state){

	}

	function myatoz_member_report_filter_form_submit($form, &$form_state){

		if ($form_state["values"]["op"] == "Reset") {

			unset ($_SESSION[ATOZ_MEMBER_ID_FILTER]);
			unset ($_SESSION[ATOZ_MEMBER_LEVEL_FILTER]);
			unset ($_SESSION[ATOZ_REDEMPTION_FREQUENCY_FILTER]);
			
		}else{
			
			$memberIdFilter = $form_state["values"]["member_id"];
			$memberLevelFilter = $form_state["values"]["member_level"];
			$frequencyFilter = $form_state["values"]["frequnecy"];
			
			if (!$memberIdFilter) {
				unset ($_SESSION[ATOZ_MEMBER_ID_FILTER]);
			}
			else {
				$_SESSION[ATOZ_MEMBER_ID_FILTER] = $memberIdFilter;
			}	

			if (!$memberLevelFilter) {
				unset ($_SESSION[ATOZ_MEMBER_LEVEL_FILTER]);
			}
			else {
				$_SESSION[ATOZ_MEMBER_LEVEL_FILTER] = $memberLevelFilter;
			}

			if (!$frequencyFilter) {
				unset ($_SESSION[ATOZ_REDEMPTION_FREQUENCY_FILTER]);
			}
			else {
				$_SESSION[ATOZ_REDEMPTION_FREQUENCY_FILTER] = $frequencyFilter;
			}		
			
		}

	}

	function frequnecy_filter($value)
	{		
	  	return $value['frequnecy'] == $_SESSION['ATOZ_REDEMPTION_FREQUENCY_FILTER'];
	}

	// function myatoz_member_report_download(){

	// 	set_time_limit (0);

	//     $header = array ("Member ID","Name","Date of Birth","Total Collected Point","Phone","Membership Type","Member Tier","City","Township","Redemption Point","Available Point","Redemption Frequency","Created By","Created Date");

	//     header('Content-type: application/csv');
	//     header ("Content-Disposition: attachment; filename=member_report.csv");
	//     echo implode (",", $header) . "\n";

	//     $query = db_select('tbl_home_points','p');
	// 	$query ->leftjoin('tbl_home_members_info','m','p.gmid = m.gmid');
	// 	$query ->leftjoin('tbl_home_users_info','i','p.created_by = i.uid');
	// 	$query ->fields('i',array('outlet'));
	// 	$query ->fields('p',array('gmid','created_date','point','invoice_id','amount'));
	// 	$query ->fields('m',array('member_level'));
	// 	$query ->orderBy('p.created_date','desc');
	// 	$result = $query ->execute();

	// 	$outlet_arr = atoz_get_outlet();
	// 	$member_level_arr = atoz_member_level();

	//     foreach ($result as $row) {

	//     	if($row->member_level == ''){
	//     		$member_level = '';
	//     	}else{
	// 			$member_level = $member_level_arr[$row->member_level];
	//     	}

	//         $point = array();
	//         $point[] = myst_clean_for_csv(date('d-M-y',strtotime($row->created_date)));
	// 		$point[] = myst_clean_for_csv($row->gmid);
	// 		$point[] = myst_clean_for_csv($row->invoice_id);
	// 		$point[] = myst_clean_for_csv($row->amount);
	// 		$point[] = myst_clean_for_csv($row->point);
	// 		$point[] = myst_clean_for_csv($outlet_arr[$row->outlet]);
	// 		$point[] = myst_clean_for_csv($member_level);

	//         echo implode (",", $point) . "\n";
	//     }
	// }

	function myatoz_member_report_download(){

		set_time_limit (0);	    

		$header = array ("Member ID","Name","Date of Birth","Total Collected Point","Phone","Membership Type","Member Tier","City","Township","Redemption Point","Available Point","Redemption Frequency","Created By","Created Date");

	    header('Content-type: application/csv');
	    header ("Content-Disposition: attachment; filename=member_report.csv");
	    echo implode (",", $header) . "\n";

	    $collect_point_query = "select i.umid, i.gmid,i.name,i.dob,i.nrc,i.phone,current_point,i.member_type,i.city,i.township,i.address,i.created_date,i.created_by,i.member_level,sum(p.point) collected_point from tbl_home_members_info i left join tbl_home_points p on i.gmid = p.gmid";

		$collect_point_query .= " group by i.gmid";

		$collect_result = db_query($collect_point_query);

		$used_point_query = "select i.umid,sum(g.gpoint) used_point from tbl_home_members_info i left join tbl_home_users_redeems ur on i.gmid = ur.gmid  left join tbl_home_gift g on ur.grid = g.grid  left join tbl_home_points p on p.gmid = i.gmid group by i.gmid";

		$used_result = db_query($used_point_query);

		$redemption_frequency_query = "select m.umid,count(ur.gmid) frequnecy from tbl_home_users_redeems ur left join tbl_home_members_info m on ur.gmid = m.gmid group by ur.gmid";

		$redemption_frequency_result = db_query($redemption_frequency_query);

		if($used_result){
			foreach($used_result as $row){
				$used_data[$row->umid] = $row->used_point;
			}
		}

		if($redemption_frequency_result){
			foreach($redemption_frequency_result as $row){
				$frequecny_data[$row->umid] = $row->frequnecy;
			}
		}

		$member_level_arr = atoz_member_level();

		$city_arr = atoz_division();

		$township_arr = atoz_townships();

	    foreach ($collect_result as $row) {

	    	if($row->member_level == ''){
	    		$member_level = '';
	    	}else{
	    		$member_level = $member_level_arr[$row->member_level];
	    	}

	    	if(isset($frequency_data[$row->umid])){
				$frequency = $frequency_data[$row->umid];
			}else{
				$frequency = 0;
			}

	        $member = array();
	        $member[] = $row->gmid;
			$member[] = $row->name;
			$member[] = $row->dob;
			$member[] = $row->nrc;
			$member[] = $row->phone;
			$member[] = $row->collected_point;
			$member[] = $row->member_type;
			$member[] = $member_level;
			$member[] = $city_arr[$row->city];
			$member[] = $township_arr[$row->township];
			$member[] = $used_data[$row->umid];
			$member[] = $row->current_point;
			$member[] = $frequency;
			$member[] = $row->created_by;
			$member[] = date('Y-m-d',strtotime($row->created_date));

	        echo implode (",", $member) . "\n";
	    }
	    exit(0);
	}
 ?>	




  