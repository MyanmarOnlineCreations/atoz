<?php 

	// Status
	define ("SUCCESS_API_STATUS", "Success");
	define ("FAILED_API_STATUS", "Fail");

	//message
	define ("NEED_POST", "Request body is missing");
	define ("WRONG_USER", "User Name or Password is incorrect. Otherwise User is not active");
	define ("NO_HEADER", "Token is required");
	define ("SESSION_EXPIRE", "SESSION is expired");
	define ("APP_UPDATE", "Need App Update");
	define ("ALREADY_BIND", "Already Bind");
	define('MANDATORY_MISSING', 'Mandatory Field is required');
	define('SECURITY_KEY_INCORRECT', 'Security key is incorrect');
	define('METHOD_NOT_ALLOW', 'Method is not allowed');
	define('SUCCESS_API', 'Success');
	define('ID_NOT_FOUND','Id does not exist');
	define('URL_NOT_FOUND', 'Url does not found');
	define('FAILED_SIGN_UP','Opp! Could not Sign Up! Please contact to administrator.');
	define('SUCCESS_SIGN_UP', 'Successfully Sign Up! We will contact you as soon as possible.');
	define('WRONG_CURR_PASS', 'Current password is incorrect.Please contact to mojoenet call center.');
	define('NEW_PASS_DO_NOT_MATCH', 'New password and Confirm password are not same.');
	define('PENDING_TICKET', 'You have pending ticket.');
	define('WRONG_USER_OR_PASSWORD', 'User Id or Password is incorrect.');

	//error code
	define ("SUCCESS_API_CODE", "000");
	define ("FAILED_API_CODE", "001");
	define ("NEED_POST_CODE", "002");
	define ("WRONG_USER_CODE", "003");
	define ("NO_HEADER_CODE", "004");
	define ("SESSION_EXPIRE_CODE", "005");
	define ("APP_UPDATE_CODE", "006");
	define ("ALREADY_BIND_CODE", "007");
	define ("MANDATORY_ERR_CODE", "008");
	define('SECURITY_ERR_CODE', '009');
	define('METHOD_NOT_ALLOW_CODE', '010');
	define('ID_NOT_FOUND_CODE','011');
	define('URL_NOT_FOUND_CODE', '404');
	define('WRONG_CURR_PASS_CODE', '012');
	define('NEW_PASS_DO_NOT_MATCH_CODE', '013');
	define('PENDING_TICKET_CODE', '014');
	define('WRONG_USER_OR_PASSWORD_CODE', '015');
	define('FAILED_SIGN_UP_CODE', '016');

	define('API_KEY', '@tO2sECuriTYkEy');

	function atoz_api_login(){

		require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');

		$method = $_SERVER['REQUEST_METHOD'];

		if($method === 'POST'){

			set_http_header($method);

			$headers = apache_request_headers();

	    	$data = json_decode(file_get_contents("php://input"));

		    $response = array();

		    $result = array();

		    $now = date('Y-m-d h:i:s');

		    $updated_status = false;

		    if(isset($headers['security_key'])){

		    	$security_key = $headers['security_key'];

		    	if(isset($data)){

			    	if($data->user_id && $data->password && $security_key && $data->app_version && $data->expo_token){

			    		if(atoz_ckeck_appversion($data->app_version)){
		    				$is_requiered_update = false;
		    			}else{
		    				$is_requiered_update = true;
		    				$updated_status = APP_UPDATE;
		    			}

		    			$isforce_update = false;

		    			if($security_key === API_KEY){

		    				$query = db_select('users','u');
		    				$query ->join('tbl_home_members_info','m','u.uid = m.uid');
		    				$query -> fields('u');
		    				$query -> fields('m',array('created_date'));
		    				$query ->condition('u.name',$data->user_id);
		    				$query ->condition('u.status',1);
		    				$query ->condition('m.status','1001');
		    				$user_data = $query ->execute()->fetchObject();

		    				// --- check password----

					    	if(user_check_password($data->password, $user_data)){

						        $token = atoz_generate_jwt_authentication($data->user_id,$data->password,$user_data->uid,$security_key);

						        $fields = array (			                    
				                    "token"         => $token,
				                    "expo_token"	=> $data->expo_token,
				                    "updated_date"  => $now,  				                                      
			                  	);			                  	

			                  	try{
			                  		db_merge ("tbl_home_authentication")
				                        ->key (array ("uid"=> $user_data->uid,"user_id"=> $user_data->name))
				                        ->fields ($fields)
				                        ->execute ();			                  		

			                  	}catch(Exception $e){
			                  		watchdog(
								        'authentication error',
								        'authentication error @error',
								        array('@error' => $e->getMessage()),
								        WATCHDOG_NOTICE
								    );

			                  		print_r($e->getMessage());

			                  	}

			                  	$result['uid'] = $user_data->uid;
			                  	$result['user_id'] = $user_data->name;
				              	$result['token'] = $token;
				              	$result['created_date'] = date('Y-m-d',strtotime($user_data->created_date));

			                    $status = SUCCESS_API_STATUS;
						        $response_code = SUCCESS_API_CODE;
				              	$description = SUCCESS_API;				              	

					    	}else{
				    			$status = FAILED_API_STATUS;
					    		$response_code = WRONG_USER_CODE;
					    		$description = WRONG_USER;
					    	}

		    			}else{
				    		$status = FAILED_API_STATUS;
				    		$response_code = SECURITY_ERR_CODE;
				    		$description = SECURITY_KEY_INCORRECT;
				    	}
		    		}else{
		    			$status = FAILED_API_STATUS;
					    $response_code = MANDATORY_ERR_CODE;
					    $description = MANDATORY_MISSING;
		    		}

		    	}else{
		    	   	$status = FAILED_API_STATUS;
				    $response_code = NEED_POST_CODE;
				    $description = NEED_POST;
			    }

		    }else{
		    	$status = FAILED_API_STATUS;
		    	$response_code = NO_HEADER_CODE;
		    	$description = "Security Key is required";
		    }

		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}

		$response = generate_response_arr($status,$response_code,$description,$is_requiered_update,$updated_status,$isforce_update,$result);

		$myJSON = json_encode($response);

		print_r($myJSON);

		exit();

	}

	function atoz_api_register(){

		$updated_status = false;

		$method = $_SERVER['REQUEST_METHOD'];

		if($method === 'POST'){

			set_http_header($method);

			$headers = apache_request_headers();

	    	$data = json_decode(file_get_contents("php://input"));

		    $response = array();

		    $result = array();

		    $now = date('Y-m-d h:i:s');

		    $name = '';
		    $dob = '';
    		$nrc = '';
    		$address = '';
    		$city = '';
    		$township = '';
    		$phone = '';
    		$password = '';
    		$confirm_password = '';
    		$gender = '';

		    if(isset($data)){

				if(isset($data->name)){
		    		$name = $data->name;
		    	}

		    	if(isset($data->dob)){
		    		$dob = $data->dob;
		    	}
		    	
		    	if(isset($data->nrc)){
		    		$nrc = $data->nrc;
		    	}

		    	if(isset($data->address)){
		    		$address = $data->address;
		    	}

		    	if(isset($data->city)){
		    		$city = $data->city;
		    	}

		    	if(isset($data->township)){
		    		$township = $data->township;
		    	}

		    	if(isset($data->phone)){
		    		$phone = $data->phone;
		    	}

		    	if(isset($data->password)){
		    		$password = $data->password;
		    	}

		    	if(isset($data->gender)){
		    		$gender = $data->gender;
		    	}

		    	if(isset($data->confirm_password)){
		    		$confirm_password = $data->confirm_password;
		    	}

		    	if($name !='' && $dob != '' && $city != '' && $township != '' && $phone != '' && $password != '' && $confirm_password != ''){

		    		if($password == $confirm_password){

		    			$gmid = atoz_get_unique_member_id("2000");

			    		$account = new stdClass();

					    $account->is_new = true;
					    // temporary name
					    $username = rand(1000, 9999) . $now;    

					    $details["mail"] = $username;
					    $details["name"] = $username;
					    $details["status"] = 0; //default status is block;
					    $details["pass"] = $password;

					    $account = user_save($account,$details);
					    $uid = $account->uid;

					    if ($uid) {
					        // creating staff account
					        $username = $gmid;
					        $email = $username.'@atoz.com';
					        $details2["mail"] = $email;
					        $details2["name"] = $username;
					        $account = user_save($account,$details2);
					    }				    

			    		$fields = array(
			    			'gmid' => $gmid,
			    			'uid' => $uid,
			    			'name' => $name,
						    'dob' => $dob,
				    		'nrc' => $nrc,
				    		'address' => $address,
				    		'city' => $city,
				    		'township' => $township,
				    		'phone' => $phone,	
				    		'gender' => $gender,
				    		'member_type' => "2000",		    		
				    		'created_date' => $now,
				    		'created_by' => $uid,
			    		);

			    		$query = db_insert('tbl_home_members_info');
			    		$query ->fields($fields);
			    		$result = $query ->execute();

			    		if($result){
			    			$status = SUCCESS_API_STATUS;
					    	$response_code = SUCCESS_API_CODE;
					    	$description = SUCCESS_SIGN_UP;
			    		}else{
			    			$status = FAILED_API_STATUS;
			    			$response_code = FAILED_SIGN_UP_CODE;
			    			$description = FAILED_SIGN_UP;
			    		}
		    		}else{
		    			$status = FAILED_API_STATUS;
				    	$response_code = FAILED_SIGN_UP_CODE;
				    	$description = "Password doesn't match";
		    		}
		    	}else{
		    		$status = FAILED_API_STATUS;
			    	$response_code = MANDATORY_ERR_CODE;
			    	$description = MANDATORY_MISSING;
		    	}    	

			}else{
				$status = FAILED_API_STATUS;
			    $response_code = NEED_POST_CODE;
			    $description = NEED_POST;
			}

		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}

		$response['status'] = $status;
		$response['response_code'] = $response_code;
		$response['description'] = $description;

		$myJSON = json_encode($response);

		print_r($myJSON);

		exit();

	}

	function atoz_api_get_member_info(){

		$updated_status = false;


		$method = $_SERVER['REQUEST_METHOD'];

		if($method === 'GET'){

			set_http_header($method);

			$headers = apache_request_headers();
	    	
		    $response = array();

		    $data = array();

		    $now = date('Y-m-d h:i:s');

		    if(isset($headers['token'])){

		    	$token = $headers['token'];
		    	$app_version = $_GET['app_version'];

		    	if($app_version && $token){

		    		if(atoz_ckeck_appversion($app_version)){
	    				$is_requiered_update = false;
	    			}else{
	    				$is_requiered_update = true;
	    				$updated_status = APP_UPDATE;
	    			}

	    			$isforce_update = false;

	    			$auth_data = is_auth_by_token($token);

	    			$uid = $auth_data['uid'];
	    			$user_id = $auth_data['user_id'];
	    			$isAuth = $auth_data['isAuth'];

	    			if($isAuth){

	    				$query = db_select('tbl_home_members_info','m');
	    				$query ->join('users','u','m.uid = u.uid');
	    				$query ->fields('m',array('name','dob','nrc','address','city','township','member_level','current_point','created_date'));
	    				$query ->addField('u','name','user_id');
	    				$query ->condition('u.status',1);
	    				$query ->condition('m.status','1001');
	    				$query ->condition('m.gmid',$user_id);
	    				$query ->condition('m.uid',$uid);

	    				$result = $query->execute()->fetchObject();

	    				$city_arr = atoz_division();
	    				$township_arr = atoz_townships();
	    				$member_level_arr = atoz_member_level();

	    				if($result){
	    					foreach($result as $key =>$value){
	    						$nvalue = '';
	    						if($key == 'city'){
	    							$nvalue = $city_arr[$value];
	    						}else if($key == 'township'){
	    							$nvalue = $township_arr[$value];
	    						}else if($key == 'created_date'){
	    							$nvalue = date('Y',strtotime($value));
	    						}else if($key == 'member_level'){
	    							$nvalue = $member_level_arr[$value];
	    						}else{
	    							$nvalue = $value;
	    						}
	    						$data[$key] = $nvalue;
	    					}
	    				}

	    				$status = SUCCESS_API_STATUS;
	    				$response_code = SUCCESS_API_CODE;
	    				$description = SUCCESS_API;

	    			}else{
	    				$status = FAILED_API_STATUS;
			            $response_code = SESSION_EXPIRE_CODE;
			            $description = SESSION_EXPIRE;
	    			}

		    	}else{
		    		$status = FAILED_API_STATUS;
				    $response_code = MANDATORY_ERR_CODE;
				    $description = MANDATORY_MISSING;
		    	}

		    }else{
				$status = FAILED_API_STATUS;
		    	$response_code = NO_HEADER_CODE;
		    	$description = NO_HEADER;
			}


		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}


		$response = generate_response_arr($status,$response_code,$description,$is_requiered_update,$updated_status,$isforce_update,$data);

		$myJSON = json_encode($response);

		print_r($myJSON);

		exit();
	
	}

	function atoz_api_get_point_history(){

		$updated_status = false;


		$method = $_SERVER['REQUEST_METHOD'];

		if($method === 'GET'){

			set_http_header($method);

			$headers = apache_request_headers();
	    	
		    $response = array();

		    $data = array();

		    $final = array();

		    $now = date('Y-m-d h:i:s');

		    if(isset($headers['token'])){

		    	$token = $headers['token'];
		    	$app_version = $_GET['app_version'];

		    	if($app_version && $token){

		    		if(atoz_ckeck_appversion($app_version)){
	    				$is_requiered_update = false;
	    			}else{
	    				$is_requiered_update = true;
	    				$updated_status = APP_UPDATE;
	    			}

	    			$isforce_update = false;

	    			$auth_data = is_auth_by_token($token);

	    			$uid = $auth_data['uid'];
	    			$user_id = $auth_data['user_id'];
	    			$isAuth = $auth_data['isAuth'];

	    			$query = db_select('tbl_home_members_info','m');
	    			$query -> condition('gmid',$user_id);
	    			$query -> fields('m',array('current_point'));
	    			$result = $query -> execute()->fetchObject();

	    			if($result->current_point){
	    				$available_point = $result->current_point;
	    			}else{
	    				$available_point = 0;
	    			}

	    			if($isAuth){

	    				$query = db_select('tbl_home_points','p');	 
	    				$query ->join('tbl_home_members_info','m','p.gmid = m.gmid');   	
	    				$query ->leftjoin('tbl_home_users_info','i','p.created_by = i.uid');
	    				$query ->leftjoin('tbl_home_outlets','o','i.outlet = o.goid');			
	    				$query ->fields('m',array('current_point'));
	    				$query ->fields('p',array('point','created_date'));
	    				$query ->fields('o',array('name'));
	    				$query ->condition('m.status','1001');
	    				$query ->condition('m.gmid',$user_id);
	    				$query ->condition('m.uid',$uid);
	    				$query ->orderBy('created_date','desc');

	    				$result = $query->execute();

	    				if($result){
	    					foreach($result as $row){

	    						$list = array();

	    						$desc = $row->point . ' points Collected at ' . $row->name . ' - ' . date('d-M-y',strtotime($row->created_date));

	    						$list['desc'] = $desc;
	    						$list['created_date'] = $row->created_date;

	    						$arr_key = date('M',strtotime($row->created_date));

	    						// print_r($arr_key);exit();

	    						$data[$arr_key][] = $list;

	    						usort($data[$arr_key], "json_array_sort");   						
	    					}
	    				}

	    				$query = db_select('tbl_home_users_redeems','r');
	    				$query ->join('tbl_home_members_info','m','r.gmid = m.gmid');
	    				$query ->join('tbl_home_gift','g','r.grid = g.grid');
	    				$query ->leftjoin('tbl_home_users_info','i','r.created_by = i.uid');
	    				$query ->leftjoin('tbl_home_outlets','o','i.outlet = o.goid');
	    				$query ->fields('m',array('current_point'));
	    				$query ->addField('g','gpoint');
	    				$query ->fields('r',array('created_date','redemption_point'));	    				
	    				$query ->fields('o',array('name'));
	    				$query ->condition('r.gmid',$user_id);
	    				$query ->condition('m.status','1001');
	    				$query ->orderBy('created_date','desc');

	    				$result = $query ->execute();	    			

	    				if($result){
	    					foreach($result as $row){
	    						
	    						$list = array();
	    						$list = array();

	    						$desc = '-' .$row->redemption_point
	    						 . ' points Redeemed at ' . $row->name . ' - ' . date('d-M-y',strtotime($row->created_date));

	    						$list['desc'] = $desc;
	    						$list['created_date'] = $row->created_date;

	    						$arr_key = date('M',strtotime($row->created_date));

	    						$data[$arr_key][] = $list;

	    						usort($data[$arr_key], "json_array_sort");

	    					}
	    				}

	    				foreach($data as $key=>$value){
	    					$final[] =  array('month'=>$key , 'rows' => $value);
	    				}

	    				$f = array();
	    				$f = array('available_point'=>$available_point,'details' => $final);

	    				$status = SUCCESS_API_STATUS;
	    				$response_code = SUCCESS_API_CODE;
	    				$description = SUCCESS_API;

	    			}else{
	    				$status = FAILED_API_STATUS;
			            $response_code = SESSION_EXPIRE_CODE;
			            $description = SESSION_EXPIRE;
	    			}

		    	}else{
		    		$status = FAILED_API_STATUS;
				    $response_code = MANDATORY_ERR_CODE;
				    $description = MANDATORY_MISSING;
		    	}

		    }else{
				$status = FAILED_API_STATUS;
		    	$response_code = NO_HEADER_CODE;
		    	$description = NO_HEADER;
			}


		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}

		$response['status'] = $status;
		$response['response_code'] = $response_code;
		$response['description'] = $description;
		$response['is_requiered_update'] = $is_requiered_update;
		$response['updated_status'] = $updated_status;
		$response['isforce_update'] = $isforce_update;
		$response['details'] = $f;

		$myJSON = json_encode($response);

		print_r($myJSON);

		exit();
	
	}

	function atoz_api_testing(){
		$response = array();
		$response['status'] = $status;
		$response['response_code'] = '000';

		$myJSON = json_encode($response);
		print_r($myJSON);
		exit();
	
	}

	function atoz_api_not_found(){

		$status = FAILED_API_STATUS;
		$response_code = URL_NOT_FOUND_CODE;
		$description = URL_NOT_FOUND;

		$response = array(
			"status"=>$status,
			"response_code"=>$response_code,
			"description"=>$description,
		);
		$myJSON = json_encode($response);
		print_r($myJSON);
		exit();
	
	}

	function atoz_ckeck_appversion($app_version){    
	    $android_app_version = $app_version;
	    $setting_query = db_select("tbl_home_setting", "s");
	    $setting_query->fields ("s");
	    $setting_query->condition("s.name", "app_version");
	    $setting_result = $setting_query->execute()->fetchObject();
	    $atoz_app_version = $setting_result->value;
	    if($atoz_app_version == $android_app_version){
	      return true;
	    }else{
	       return false;
	    }
	}

	function atoz_generate_jwt_authentication($user_id,$password,$customer_id,$security_key){
	   
	    require_once ("jwt_auth.inc");

	    if(function_exists('mcrypt_create_iv')){
	        $tokenId    = base64_encode(mcrypt_create_iv(32,MCRYPT_DEV_URANDOM));
	    }else if(function_exists('openssl_random_pseudo_bytes')){        
	        $tokenId = base64_encode(openssl_random_pseudo_bytes(32));        
	    }else{
	        $tokenId = rand();
	    }

	    $issuedAt   = time();
	    $notBefore  = $issuedAt + 10;             //Adding 10 seconds
	    $expire     = $notBefore + 60;            // Adding 60 seconds
	    $serverName = ATOZ_SERVER_NAME; // Retrieve the server name from config file       

	    /*
	     * Create the token as an array
	     */
	    $data = [
	        'iat'  => $issuedAt,         // Issued at: time when the token was generated
	        'jti'  => $tokenId,          // Json Token Id: an unique identifier for the token
	        'iss'  => $serverName,       // Issuer
	        'nbf'  => $notBefore,        // Not before
	        'exp'  => $expire,           // Expire
	        'data' => [                  // Data related to the signer user
	            'user_id'   => $user_id, // userid from the users table
	            'password'  => $password,
	            'customer_id'=> $customer_id,
	        ]
	    ];


	    $secretKey = base64_decode($security_key);

	    $jwt = jwt_encode(
	        $data,      //Data to be encoded in the JWT
	        $secretKey, // The signing key
	        'HS512'     // Algorithm used to sign the token, see https://tools.ietf.org/html/draft-ietf-jose-json-web-algorithms-40#section-3
	    );
	    return $jwt;

	}

	function atoz_api_get_home(){

		$updated_status = false;

		$response = array();

		$method = $_SERVER['REQUEST_METHOD'];

		if($method === 'GET'){

			set_http_header($method);

			$headers = apache_request_headers();
	    	
		    $response = array();

		    $data = array();

		    $d = array();

		    $now = date('Y-m-d h:i:s');

		    if(isset($headers['token'])){

		    	$token = $headers['token'];
		    	$app_version = $_GET['app_version'];

		    	if($app_version && $token){

		    		if(atoz_ckeck_appversion($app_version)){
	    				$is_requiered_update = false;
	    			}else{
	    				$is_requiered_update = true;
	    				$updated_status = APP_UPDATE;
	    			}

	    			$isforce_update = false;

	    			$auth_data = is_auth_by_token($token);

	    			$uid = $auth_data['uid'];
	    			$user_id = $auth_data['user_id'];
	    			$isAuth = $auth_data['isAuth'];

	    			$query = db_select('tbl_home_members_info','m');
	    			$query -> condition('gmid',$user_id);
	    			$query -> fields('m',array('current_point'));
	    			$result = $query -> execute()->fetchObject();

	    			if($result->current_point){
	    				$available_point = $result->current_point;
	    			}else{
	    				$available_point = 0;
	    			}

	    			if($isAuth){
	    				$query = db_select('tbl_home_promotion','p');
	    				$query ->fields('p');
	    				$query ->orderBy('created_date','desc');
	    				$result = $query -> execute();

	    				$ads_type_arr = atoz_ads_type();

	    				$path = atoz_web_path ('promo_slide');


	    				if($result){
	    					foreach($result as $row){
	    						foreach($row as $key => $value){
	    							if($key == 'ads_type'){
	    								$nvalue = $ads_type_arr[$value];
	    							}else if($key == 'image_en' || $key == 'image_mm'){
	    								$nvalue = $path . $value;
	    							}else{
	    								$nvalue = $value;
	    							}
		    						$list[$key] = $nvalue;
		    					}

	    					$d[] = $list;

	    					}
	    				}

	    				$data = array('available_point'=>$available_point,'details'=>$d);

	    				$status = SUCCESS_API_STATUS;
	    				$response_code = SUCCESS_API_CODE;
	    				$description = SUCCESS_API;

	    			}else{
	    				$status = FAILED_API_STATUS;
	    				$response_code = SESSION_EXPIRE_CODE;
	    				$description = SESSION_EXPIRE;
	    			}

		    	}else{
		    		$status = FAILED_API_STATUS;
		    		$response_code = MANDATORY_ERR_CODE;
		    		$description = MANDATORY_MISSING;
		    	}

		    }else{
		    	$status = FAILED_API_STATUS;
		    	$response_code = NO_HEADER_CODE;
		    	$description = NO_HEADER;
		    }


		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}

		$response = generate_response_arr($status,$response_code,$description,$is_requiered_update,$updated_status,$isforce_update,$data);

		$myJSON = json_encode($response);
		print_r($myJSON);
		exit();
		
	}

	function atoz_api_get_promotions(){

		$updated_status = false;


		$response = array();

		$method = $_SERVER['REQUEST_METHOD'];

		if($method === 'GET'){

			set_http_header($method);

			$headers = apache_request_headers();
	    	
		    $response = array();

		    $data = array();
		    $d = array();

		    $now = date('Y-m-d h:i:s');

		    if(isset($headers['token'])){

		    	$token = $headers['token'];
		    	$app_version = $_GET['app_version'];

		    	if($app_version && $token){

		    		if(atoz_ckeck_appversion($app_version)){
	    				$is_requiered_update = false;
	    			}else{
	    				$is_requiered_update = true;
	    				$updated_status = APP_UPDATE;
	    			}

	    			$isforce_update = false;

	    			$auth_data = is_auth_by_token($token);

	    			$uid = $auth_data['uid'];
	    			$user_id = $auth_data['user_id'];
	    			$isAuth = $auth_data['isAuth'];

	    			if($isAuth){
	    				$query = db_select('tbl_home_promotion','p');
	    				$query ->fields('p');
	    				$query ->condition('ads_type','9002' );
	    				$query ->orderBy('created_date','desc');
	    				$result = $query -> execute();

	    				$ads_type_arr = atoz_ads_type();

	    				$path = atoz_web_path ('promo_slide');

	    				if($result){
	    					foreach($result as $row){	    						
	    						foreach($row as $key => $value){
	    							$nvalue = '';
	    							if($key == 'ads_type'){
	    								$nvalue = $ads_type_arr[$value];
	    							}else if($key == 'image_en' || $key == 'image_mm'){
	    								$nvalue = $path . $value;
	    							}else{
	    								$nvalue = $value;
	    							}
		    						$list[$key] = $nvalue;
		    					}

	    					$data[] = $list;

	    					}
	    				}

	    				// $data = array('available_point'=>$available_point,'details'=>$d);

	    				$status = SUCCESS_API_STATUS;
	    				$response_code = SUCCESS_API_CODE;
	    				$description = SUCCESS_API;

	    			}else{
	    				$status = FAILED_API_STATUS;
	    				$response_code = SESSION_EXPIRE_CODE;
	    				$description = SESSION_EXPIRE;
	    			}

		    	}else{
		    		$status = FAILED_API_STATUS;
		    		$response_code = MANDATORY_ERR_CODE;
		    		$description = MANDATORY_MISSING;
		    	}

		    }else{
		    	$status = FAILED_API_STATUS;
		    	$response_code = NO_HEADER_CODE;
		    	$description = NO_HEADER;
		    }


		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}

		$response = generate_response_arr($status,$response_code,$description,$is_requiered_update,$updated_status,$isforce_update,$data);

		$myJSON = json_encode($response);
		print_r($myJSON);
		exit();

	}

	function atoz_api_change_password(){

		require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');

		$method = $_SERVER['REQUEST_METHOD'];

		$response = array();

		$updated_status = false;

		if($method === 'POST'){

			set_http_header($method);

			$headers = apache_request_headers();

	    	$data = json_decode(file_get_contents("php://input"));

		    $response = array();

		    $result = array();

		    $now = date('Y-m-d h:i:s');

		    if(isset($headers['token'])){

		    	$token = $headers['token'];

		    	if(isset($data)){

			    	if($data->old_password && $data->new_password){

			    		if(atoz_ckeck_appversion($data->app_version)){
			    				$is_requiered_update = false;
			    			}else{
			    				$is_requiered_update = true;
			    				$updated_status = APP_UPDATE;
			    			}
			    			$isforce_update = false;

			    			$auth_data = is_auth_by_token($token);

			    			$uid = $auth_data['uid'];
			    			$user_id = $auth_data['user_id'];
			    			$isAuth = $auth_data['isAuth'];

			    			if($isAuth){

			    				$query = db_select('users','u');
			    				$query -> fields('u');
			    				$query ->condition('u.name',$user_id);
			    				$query ->condition('u.status',1);
			    				$user_data = $query ->execute()->fetchObject();

			    				// --- check password----
						    	if(user_check_password($data->old_password, $user_data)){

						    		$fields = array();

						    		$existingUser = user_load($user_data->uid);

						    		$existingUser->pass = $data->new_password;

									user_save((object) array('uid' => $existingUser->uid), (array) $existingUser);

				                    $status = SUCCESS_API_STATUS;
							        $response_code = SUCCESS_API_CODE;
					              	$description = SUCCESS_API;				              	

						    	}else{
					    			$status = FAILED_API_STATUS;
						    		$response_code = WRONG_USER_CODE;
						    		$description = WRONG_USER;
						    	}

			    			}else{
					    		$status = FAILED_API_STATUS;
					    		$response_code = SESSION_EXPIRE_CODE;
					    		$description = SESSION_EXPIRE;
					    	}
			    		
		    		}else{
		    			$status = FAILED_API_STATUS;
					    $response_code = MANDATORY_ERR_CODE;
					    $description = MANDATORY_MISSING;
		    		}

		    	}else{
		    	   	$status = FAILED_API_STATUS;
				    $response_code = NEED_POST_CODE;
				    $description = NEED_POST;
			    }

		    }else{
		    	$status = FAILED_API_STATUS;
		    	$response_code = NO_HEADER_CODE;
		    	$description = "Security Key is required";
		    }

		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}

		$response['status'] = $status;
		$response['response_code'] = $response_code;
		$response['description'] = $description;

		$myJSON = json_encode($response);
		print_r($myJSON);
		exit();

	}

	function atoz_api_get_ddl_data(){

		$method = $_SERVER['REQUEST_METHOD'];

		$member_level_arr = atoz_member_level();
		$member_type_arr = atoz_member_types();
		$status_arr = atoz_member_status();
		
		$city_arr = atoz_division();
		$data = array();

		$method = $_SERVER['REQUEST_METHOD'];

		if($method === 'GET'){

			set_http_header($method);

			$data['member_level'] = [];
			$data['member_type'] = [];
			$data['status'] = [];
			$data['township'] = [];
			$data['city'] = [];

			foreach($member_level_arr as $key => $value){
				$list = array();
				$list['key'] = $key;
				$list['value'] = $value;

				$data['member_level'][] = $list;
			}

			foreach($member_type_arr as $key => $value){
				$list = array();
				$list['key'] = $key;
				$list['value'] = $value;

				$data['member_type'][] = $list;
			}

			foreach($status_arr as $key => $value){
				$list = array();
				$list['key'] = $key;
				$list['value'] = $value;

				$data['status'][] = $list;
			}

			for($i = 1 ; $i <= 15 ; $i ++){

				$township_arr = atoz_townships($i);

				foreach($township_arr as $key => $value){
					$list = array();

					$list['key'] = $key;
					$list['value'] = $value;
					$list['division'] = $i;

					$data['township'][] = $list;
				}
			}

			foreach($city_arr as $key => $value){
				$list = array();
				$list['key'] = $key;
				$list['value'] = $value;

				$data['city'][] = $list;
			}

			$status = SUCCESS_API_STATUS;
			$response_code = SUCCESS_API_STATUS;
			$description = SUCCESS_API;

		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}

		$response = array();
		$response['status'] = $status;
		$response['response_code'] = $response_code;
		$response['description'] = $description;
		$response['details'] = $data;		

		$myJSON = json_encode($response);

		print_r($myJSON);

		exit();

	}

	function atoz_api_get_outlet_location(){ 

		//outlet eng

		$outlet1 = array();
		$outlet1['location'] = 'Yangon Head Office';
		$outlet1['address'] = 'No.70/71 A, Lan Thit Road, In front of Arlein Ngar Sint Pagoda, Insein Township, Yangon.';
		$outlet1['phnumber'] = '01-7640371 , 01-7640370';

		$outlet2 = array();
		$outlet2['location'] = 'Yangon Branch';
		$outlet2['address'] = '(G-17/18), Two Way Road, Thiri Yadana Wholesale Market, North Okkalapa, Yangon.';
		$outlet2['phnumber'] = '09 795 903 250, 09 780 900 062';

		$outlet3 = array();
		$outlet3['location'] = 'Thanlyin Branch';
		$outlet3['address'] = 'No.242, Ton 100 San Sat Won, 18 Kone Htate, Thanlyin.';
		$outlet3['phnumber'] = '09 445 44 1119, 09 445 44 1117, 09 740 913 058, 09 740 886 041.';

		$outlet4 = array();
		$outlet4['location'] = 'Mandalay Branch';
		$outlet4['address'] = 'No.(DaDa – 8/12, 15, 16, 17, Corner of 62 & 131 Street, Pyi Gyi Takhon Township, Mandalay.';
		$outlet4['phnumber'] = '02-515 4890, 02-515 4891, 02-515 4696, 02-515 4784, 02-515 4748.';

		$outlet5 = array();
		$outlet5['location'] = 'Taunggyi Branch (1)';
		$outlet5['address'] = 'No.104, Hnin Si Street, Pyi Taw Thar Ward, Taunggyi.';
		$outlet5['phnumber'] = '081-212 2513';

		$outlet6 = array();
		$outlet6['location'] = 'Taunggyi Branch (2)';
		$outlet6['address'] = 'No.(267/B), West Circular Road, Chan Myae Tharzi Ward, Taunggyi.';
		$outlet6['phnumber'] = '081-205 255, 081-205 155';

		$outlet7 = array();
		$outlet7['location'] = 'Mawlamyine Branch';
		$outlet7['address'] = 'No.(37,Ka) C, Kyaikkhame Main Road, Zay Kyo Ward, Mawlamyine.';
		$outlet7['phnumber'] = '09 445 44 1112, 09 445 44 1113, 09 780 333 921, 09 780 333 985';

		$outlet8 = array();
		$outlet8['location'] = 'Monywa Branch';
		$outlet8['address'] = 'No (12/D), Land No. 670, Pyi Htaung Su Main Road, Pa Douk Kone Ward, Monywa.';
		$outlet8['phnumber'] = '09 441 799 993, 09 441 799 995';

		$outlet9 = array();
		$outlet9['location'] = 'Taungoo Branch';
		$outlet9['address'] = 'No.(489/490), Yangon-Mandalay Highway Road , the corner of Bagan Street & Inwa Street ,No. 22 Min Gyi Nyo Ward, Taungoo Township.';
		$outlet9['phnumber'] = '09 445 44 2220, 09 445 44 2221, 054 25678';

		$outlet_en_arr = array();

		$outlet_en_arr[] = $outlet1;
		$outlet_en_arr[] = $outlet2;
		$outlet_en_arr[] = $outlet3;
		$outlet_en_arr[] = $outlet4;
		$outlet_en_arr[] = $outlet5;
		$outlet_en_arr[] = $outlet6;
		$outlet_en_arr[] = $outlet7;
		$outlet_en_arr[] = $outlet8;
		$outlet_en_arr[] = $outlet9;

		// outlet mm

		$outlet1 = array();
		$outlet1['location'] = 'ရန်ကုန်ရုံးချုပ်';
		$outlet1['address'] = 'အမှတ် (၇၀/၇၁ က), လမ်းသစ်လမ်း, အာလိန်ငါးဆင့်ဘုရားရှေ့, အင်းစိန်မြို့နယ်, ရန်ကုန်မြို့။';
		$outlet1['phnumber'] = '၀၁ ၇၆၄၀ ၃၇၁, ၀၁ ၇၆၄၀ ၃၇၀';

		$outlet2 = array();
		$outlet2['location'] = 'ရန်ကုန်ဆိုင်ခွဲ';
		$outlet2['address'] = '(ဂျီ-၁၇/၁၈)၊ Two Way လမ်းမ၊ သီရိရတနာ‌လက္ကားဝင်း၊ မြောက်ဥက္ကလာပမြို့နယ်၊ ရန်ကုန်မြို့။';
		$outlet2['phnumber'] = '(၀၉) ၇၉၅ ၉၀၃ ၂၅၀, (၀၉) ၇၈၀ ၉၀၀ ၀၆၂';

		$outlet3 = array();
		$outlet3['location'] = 'သန်လျင်ဆိုင်ခွဲ';
		$outlet3['address'] = 'အမှတ် (၂၄၂)၊ တန်တစ်ရာဆန်စက်ဝန်း၊ ၁၈ ကုန်းထိပ်၊ သန်လျင်မြို့နယ်၊';
		$outlet3['phnumber'] = '(၀၉) ၄၄၅ ၄၄၁ ၁၁၉, (၀၉) ၄၄၅ ၄၄၁ ၁၁၇, (၀၉) ၇၄၀ ၉၁၃ ၀၅၈, (၀၉) ၇၄၀ ၈၈၆ ၀၄၁';

		$outlet4 = array();
		$outlet4['location'] = 'မန္တလေးဆိုင်ခွဲ ';
		$outlet4['address'] = 'အမှတ် (ဓဓ-၈/၁၂,၁၅,၁၆,၁၇), ၆၂ လမ်း x ၁၃၁ လမ်းထောင့် ,ပြည်ကြီးတံခွန်မြို့နယ်,';
		$outlet4['phnumber'] = '- (၀၂) ၅၁၅ ၄၈၉၀, (၀၂) ၅၁၅ ၄၈၉၁, (၀၂) ၅၁၅ ၄၆၉၆, (၀၂) ၅၁၅ ၄၇၈၄, (၀၂) ၅၁၅ ၄၇၄၈';

		$outlet5 = array();
		$outlet5['location'] = 'တောင်ကြီးဆိုင်ခွဲ (၁)';
		$outlet5['address'] = 'အမှတ်(၁၀၄)၊ နှင်းဆီလမ်း၊‌ ပြည်တော်သာရပ်ကွက်၊ တောင်ကြီးမြို့။';
		$outlet5['phnumber'] = ' (၀၈၁) ၂၁၂၂၂၅၁၃၊ (၀၈၁) ၂၀၅၁၅၅';

		$outlet6 = array();
		$outlet6['location'] = 'တောင်ကြီးဆိုင်ခွဲ (၂)';
		$outlet6['address'] = 'အမှတ် (၂၆၇/ခ)၊ အနောက်မြို့ပတ်လမ်း၊ ချမ်းမြေ့သာစည်ရပ်ကွက်၊ တောင်ကြီးမြို့။	';
		$outlet6['phnumber'] = '(၀၈၁) ၂၀၅၂၅၅၊ (၀၈၁)၂၀၅၁၅၅';

		$outlet7 = array();
		$outlet7['location'] = 'မော်လမြိုင်ဆိုင်ခွဲ';
		$outlet7['address'] = 'အမှတ် (၃၇/က/စီ)၊ ကျိုက်ခမီလမ်း၊ စေျ ကြိုရပ်၊ မော်လမြိုင်မြို့။';
		$outlet7['phnumber'] = '(၀၉) ၄၄၅ ၄၄၁ ၁၁၂, (၀၉) ၄၄၅ ၄၄၁ ၁၁၃, (၀၉) ၇၈၀ ၃၃၃ ၉၂၁, (၀၉) ၇၈၀ ၃၃၃ ၉၈၅';

		$outlet8 = array();
		$outlet8['location'] = 'မုံရွာဆိုင်ခွဲ';
		$outlet8['address'] = 'အမှတ် (၁၂/ဃ)၊ အကွက်အမှတ် ၆၇၀၊ ပြည်ထောင်စုလမ်းမကြီးဘေး၊ ပိတောက်ကုန်းရပ်ကွက်၊ မုံရွာမြို့။';
		$outlet8['phnumber'] = '(၀၉) ၄၄၁ ၇၉၉ ၉၉၃, (၀၉) ၄၄၁ ၇၉၉ ၉၉၅';

		$outlet9 = array();
		$outlet9['location'] = 'တောင်ငူဆိုင်ခွဲ';
		$outlet9['address'] = 'အမှတ်(၄၈၉/၄၉၀)၊ ရန်ကုန်-မန္တလေး လမ်းမကြီးဘေး၊ ပုဂံလမ်း နှင့် အင်းဝလမ်းကြား၊ အရှေ့ခြမ်း၊ အမှတ် (၂၂) မင်းကြီးညိုရပ်ကွက်၊ တောင်ငူမြို့။';
		$outlet9['phnumber'] = '(၀၉) ၄၄၅ ၄၄ ၂၂၂၀, (၀၉) ၄၄၅ ၄၄ ၂၂၂၁, (၀၅၄) ၂၅၆၇၈';

		$outlet_mm_arr = array();

		$outlet_mm_arr[] = $outlet1;
		$outlet_mm_arr[] = $outlet2;
		$outlet_mm_arr[] = $outlet3;
		$outlet_mm_arr[] = $outlet4;
		$outlet_mm_arr[] = $outlet5;
		$outlet_mm_arr[] = $outlet6;
		$outlet_mm_arr[] = $outlet7;
		$outlet_mm_arr[] = $outlet8;
		$outlet_mm_arr[] = $outlet9;

		$data = array();

		$method = $_SERVER['REQUEST_METHOD'];

		if($method === 'GET'){

			set_http_header($method);		

			$data[] = array('type'=>'en','detail'=>$outlet_en_arr);
			$data[] = array('type'=>'my','detail'=>$outlet_mm_arr);

			$status = SUCCESS_API_STATUS;
			$response_code = SUCCESS_API_STATUS;
			$description = SUCCESS_API;

		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}

		$response = array();
		$response['status'] = $status;
		$response['response_code'] = $response_code;
		$response['description'] = $description;
		$response['details'] = $data;		

		$myJSON = json_encode($response);

		print_r($myJSON);

		exit();

	}
	
	function set_http_header($method){
		header("Access-Control-Allow-Origin: *");
	    header("Content-Type: application/json; charset=UTF-8");
	    header("Access-Control-Allow-Methods: ". $method);
	    header("Access-Control-Max-Age: 3600");
	    header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, X-Requested-With");
	
	}

	function generate_response_arr($status,$response_code,$description,$is_requiered_update,$updated_status,$isforce_update,$data = array()){

		$response = array();
				
		$response['status'] = $status;
	    $response['response_code'] = $response_code;
	    $response['description'] = $description;
	    if($response_code == SUCCESS_API_CODE){
	    	$response['is_requiered_update'] = $is_requiered_update;
	    	$response['isforce_update'] = $isforce_update;
	    	if($is_requiered_update){
		    	$response['updated_status'] = $updated_status;
		    }
		    $response['details'] = $data; 
	    }       
	    
	    return $response;
	}

	function generate_response_arr_without_details($status,$response_code,$description,$is_requiered_update=false,$updated_status='',$isforce_update=false){
		$response = array();
		$response['status'] = $status;
		$response['response_code'] = $response_code;
		$response['description'] = $description;
		$response['is_requiered_update'] = $is_requiered_update;
		if($updated_status != ''){
			$response['updated_status'] = $updated_status;
		}
		$response['isforce_update'] = $isforce_update;
		
		return $response;
	}

	function is_auth_by_token($token){

		$response = array();

		$uid = 0;
		$user_id = '';
		$isAuth = false;

		$query = db_select('tbl_home_authentication','a');
		$query ->condition('a.token',$token);
		$query ->fields('a',array('uid','user_id'));
		$result = $query ->execute()->fetchObject();

		if($result){
			$uid = $result->uid;
			$user_id = $result->user_id;
		}

	    if($uid > 0){
	    	$isAuth = true;
	    }

	    $response['isAuth'] = $isAuth;
    	$response['uid'] = $uid;
    	$response['user_id'] = $user_id;

        return $response;
	
	}

	function json_array_sort($a, $b)
    {
        if ($a['created_date'] == $b['created_date']) return 0;
        return ($a['created_date'] < $b['created_date']) ? 1 : -1;

    }

    function atoz_api_sent_noti(){
    	// watchdog(
     //        'submit',
     //        'submit @submit',
     //        array('@submit' => 'got submit'),
     //        WATCHDOG_NOTICE
     //    );

	    // $payload = array(
	    //     'to' => [

		   //  ],
	    //     'sound' => 'default',
	    //     'body' => 'notification from web',
	    // );

	    // $curl = curl_init();

	    // curl_setopt_array($curl, array(
	    //   CURLOPT_URL => "https://exp.host/--/api/v2/push/send",
	    //   CURLOPT_RETURNTRANSFER => true,
	    //   CURLOPT_SSL_VERIFYHOST=> false,
     //  	  CURLOPT_SSL_VERIFYPEER=> false,
	    //   CURLOPT_ENCODING => "",
	    //   CURLOPT_MAXREDIRS => 10,
	    //   CURLOPT_TIMEOUT => 30,
	    //   CURLOPT_HTTP_VERSION => CURL_HTTP_VERSION_1_1,
	    //   CURLOPT_CUSTOMREQUEST => "POST",
	    //   CURLOPT_POSTFIELDS => json_encode($payload),
	    //   CURLOPT_HTTPHEADER => array(
	    //     "Accept: application/json",
	    //     "Accept-Encoding: gzip, deflate",
	    //     "Content-Type: application/json",
	    //     "cache-control: no-cache",
	    //     "host: exp.host"
	    //   ),
	    // ));

	    // $response = curl_exec($curl);
	    // $err = curl_error($curl);

	    // curl_close($curl);

	    // if ($err) {
	    //   // echo "cURL Error #:" . $err;
	    //   watchdog(
	    //         'cURL error',
	    //         'cURL error @error',
	    //         array('@error' => $err),
	    //         WATCHDOG_NOTICE
	    //     );
	    // } else {
	    //     watchdog(
	    //         'curl response',
	    //         'curl response @response',
	    //         array('@response' => $response),
	    //         WATCHDOG_NOTICE
	    //     );
	    // }


	    for($i = 10; $i <= 110 ; $i++){
	    	$query = "insert into tbl_home_authentication values (".$i.",0,0,'',now(),now(),1,1,'','ExponentPushToken[IqB2CwC9AjsigcT_iqc78N]')";
	    	db_query($query);
	    }

	    for($i = 111; $i <= 211 ; $i++){
	    	$query = "insert into tbl_home_authentication values (".$i.",0,0,'',now(),now(),1,1,'','ExponentPushToken[rp6YS_IVTeJOd9jQf0hs0i]')";
	    	db_query($query);
	    }

	    print_r('success');exit();
    }

    function atoz_api_get_notifications(){

    	$updated_status = false;

		$response = array();

		$method = $_SERVER['REQUEST_METHOD'];

		if($method === 'GET'){

			set_http_header($method);

			$headers = apache_request_headers();
	    	
		    $response = array();

		    $data = array();
		    $d = array();

		    $now = date('Y-m-d h:i:s');

		    if(isset($headers['token'])){

		    	$token = $headers['token'];
		    	$app_version = $_GET['app_version'];

		    	if($app_version && $token){

		    		if(atoz_ckeck_appversion($app_version)){
	    				$is_requiered_update = false;
	    			}else{
	    				$is_requiered_update = true;
	    				$updated_status = APP_UPDATE;
	    			}

	    			$isforce_update = false;

	    			$auth_data = is_auth_by_token($token);

	    			$uid = $auth_data['uid'];
	    			$user_id = $auth_data['user_id'];
	    			$isAuth = $auth_data['isAuth'];
	    			$created_date = $_GET['created_date'];

	    			if($isAuth){
	    				$query = db_select('tbl_home_notifications','n');
	    				$query ->fields('n');
	    				$query ->condition('creation_date',$created_date,'>=');
	    				$query ->orderBy('creation_date','desc');
	    				$result = $query -> execute();

	    				if($result){
	    					foreach($result as $row){	    						
	    						foreach($row as $key => $value){
	    							
		    						$list[$key] = $value;
		    					}

	    					$data[] = $list;

	    					}
	    				}

	    				$status = SUCCESS_API_STATUS;
	    				$response_code = SUCCESS_API_CODE;
	    				$description = SUCCESS_API;

	    			}else{
	    				$status = FAILED_API_STATUS;
	    				$response_code = SESSION_EXPIRE_CODE;
	    				$description = SESSION_EXPIRE;
	    			}

		    	}else{
		    		$status = FAILED_API_STATUS;
		    		$response_code = MANDATORY_ERR_CODE;
		    		$description = MANDATORY_MISSING;
		    	}

		    }else{
		    	$status = FAILED_API_STATUS;
		    	$response_code = NO_HEADER_CODE;
		    	$description = NO_HEADER;
		    }


		}else{
			$status = FAILED_API_STATUS;
			$response_code = METHOD_NOT_ALLOW_CODE;
			$description = METHOD_NOT_ALLOW;
		}

		$response = generate_response_arr($status,$response_code,$description,$is_requiered_update,$updated_status,$isforce_update,$data);

		$myJSON = json_encode($response);
		print_r($myJSON);
		exit();

    }



?>

