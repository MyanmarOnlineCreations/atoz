
CREATE TABLE `tbl_home_invoice` (
  `giid` varchar(64) NOT NULL,
  `customer_id` varchar(64) DEFAULT NULL,
  `amount` decimal(11,0) DEFAULT NULL,
  `created_date` datetime DEFAULT NULL,
  `created_by` int(11) DEFAULT NULL,
  `updated_date` datetime DEFAULT NULL,
  `updated_by` int(11) DEFAULT NULL,
  PRIMARY KEY (`giid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `tbl_home_points` (
  `pid` int(11) NOT NULL AUTO_INCREMENT,  
  `gmid` varchar(64) DEFAULT NULL,
  `point` int(11) DEFAULT NULL,
  `invoice_id` varchar(64) DEFAULT NULL,
  `amount` decimal DEFAULT 0,
  `created_date` datetime DEFAULT NULL,
  `updated_date` datetime DEFAULT NULL,
  `created_by` varchar(64) DEFAULT NULL,
  `updated_by` varchar(64) DEFAULT NULL,  
  PRIMARY KEY (`pid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `tbl_home_redeem` (
  `grid` varchar(64) NOT NULL,
  `rname` varchar(64) DEFAULT NULL,
  `rpoint` int(11) DEFAULT NULL,
  `created_date` datetime DEFAULT NULL,
  `updated_date` datetime DEFAULT NULL,
  `created_by` int(11) DEFAULT NULL,
  `updated_by` int(11) DEFAULT NULL,
  PRIMARY KEY (`grid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;



CREATE TABLE `tbl_home_member_level` (
  `gmlid` varchar(64) NOT NULL,
  `member_tier` varchar(64) DEFAULT NULL,
  `type` int(1) DEFAULT NULL,
  `start_point` int(11) DEFAULT 0,
  `end_point` int(11) DEFAULT 0,
  `created_date` datetime DEFAULT CURRENT_TIMESTAMP,
  `created_by` int(11) DEFAULT NULL,
  `updated_date` datetime DEFAULT CURRENT_TIMESTAMP,
  `updated_by` int(11) DEFAULT NULL,
  PRIMARY KEY (`gmlid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `tbl_home_users_info` (
  `guid` varchar(64) NOT NULL,
  `name` varchar(64) DEFAULT NULL,
  `dob` date DEFAULT NULL,
  `nrc` varchar(32) DEFAULT NULL,
  `address` text,
  `phone` varchar(15) DEFAULT NULL,
  `email` varchar(64) DEFAULT NULL,
  `password` varchar(64) DEFAULT NULL,
  `status` int(11) DEFAULT NULL,
  `type` int(5) DEFAULT NULL,
  `customer_id` varchar(64) DEFAULT NULL,
  `created_date` datetime DEFAULT NULL,
  `created_by` varchar(64) DEFAULT NULL,
  `updated_date` datetime DEFAULT NULL,
  `updated_by` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `tbl_home_members_info` (
  `umid` INT(11) NOT NULL AUTO_INCREMENT,
  `gmid` varchar(64) NOT NULL,
  `name` varchar(64) DEFAULT NULL,
  `dob` date DEFAULT NULL,
  `nrc` varchar(32) DEFAULT NULL,
  `address` text,
  `city` int(11),
  `township` int(11),
  `phone` varchar(15) DEFAULT NULL,
  `password` varchar(64) DEFAULT NULL,
  `status` int(11) DEFAULT NULL,
  `member_type` int(5) DEFAULT NULL,  
  `member_level` varchar(64) DEFAULT NULL,
  `current_point` int(11) DEFAULT 0,
  `created_date` datetime DEFAULT NULL,
  `created_by` varchar(64) DEFAULT NULL,
  `updated_date` datetime DEFAULT NULL,
  `updated_by` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`umid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `tbl_home_users_redeems` (
  `gurid` varchar(64) NOT NULL,
  `grid` varchar(64) DEFAULT NULL,
  `customer_id` varchar(64) DEFAULT NULL,
  `created_date` datetime DEFAULT NULL,
  `updated_date` datetime DEFAULT NULL,
  `created_by` int(11) DEFAULT NULL,
  `updated_by` int(11) DEFAULT NULL,
  PRIMARY KEY (`gurid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `tbl_home_outlets` (
  `goid` varchar(64) NOT NULL,
  `name` varchar(64) DEFAULT NULL,
  `township` int(11) DEFAULT NULL,
  `city` int(11) DEFAULT NULL,
  `created_date` datetime DEFAULT NULL,
  `updated_date` datetime DEFAULT NULL,
  `created_by` int(11) DEFAULT NULL,
  `updated_by` int(11) DEFAULT NULL,
  PRIMARY KEY (`goid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

alter table tbl_home_users_info add column goid varchar(64);

alter table tbl_home_users_info add column gmlid varchar(64);

alter table tbl_home_outlets add column status int(1) default 1 after city;

CREATE TABLE `tbl_home_promotion` (
  `id` INT(11) NOT NULL AUTO_INCREMENT,
  `promo_id` VARCHAR(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `name` VARCHAR(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `type` VARCHAR(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `image_en` VARCHAR(255) CHARACTER SET utf8 DEFAULT NULL, 
  `image_mm` VARCHAR(255) CHARACTER SET utf8 DEFAULT NULL,  
  `image_cn` VARCHAR(255) CHARACTER SET utf8 DEFAULT NULL,
  `created_date` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `created_by` INT(11),
  `modified_date` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `modified_by` INT(11),
  PRIMARY KEY (`id`)
);

CREATE TABLE `tbl_home_notifications` (
  `nid` INT(11) NOT NULL AUTO_INCREMENT,
  `notification_id` VARCHAR(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `title` VARCHAR(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `message` TEXT CHARACTER SET utf8 COLLATE utf8_unicode_ci,
  `creation_date` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `created_by` int(11),
  PRIMARY KEY (`nid`)
);

CREATE TABLE `tbl_home_city`(
  `cid` int(11) AUTO_INCREMENT,
  `city_name` varchar(64) DEFAULT NULL,
  PRIMARY KEY cid_key (cid)
);
CREATE TABLE `tbl_home_township`(
  `tid` int(11) AUTO_INCREMENT,
  `cid` int(11),
  `township_name` varchar(64) DEFAULT NULL,
  PRIMARY KEY tid_key (tid),
  KEY cid_key (cid)
);

INSERT INTO tbl_home_city (city_name) VALUES 
("Ayeyarwady"),("Bago"),("Chin"),("Kachin"),("Kayah"),("Kayin"),("Magway"),("Mandalay"),("Mon"),("Nay Pyi Taw"),("Rakhine"),("Sagaing"),("Shan"),("Tanintharyi"),("Yangon");

INSERT INTO tbl_home_township (cid,township_name) VALUES 
(1,"Bogale"),(1,"Danubyu"),(1,"Dedaye"),(1,"Einme"),(1,"Du Yar"),(1,"Hinthada"),(1,"Ta Loke Htaw"),(1,"Htoogyi"),(1,"Ingapu"),(1,"Me Za Li Kone"),(1,"Kangyidaunt"),(1,"Kyaiklat"),(1,"Batye"),(1,"Kyangin"),(1,"Kyaunggon"),(1,"Ahtaung"),(1,"Kyonpyaw"),(1,"Hainggyikyun"),(1,"Labutta (3) Mile"),(1,"Labutta"),(1,"Pyinsalu"),(1,"Lemyethna"),(1,"Maubin"),(1,"Mawlamyinegyun"),(1,"In Pin"),(1,"Kanaung"),(1,"Myanaung"),(1,"Myaungmya"),(1,"Hainggyikyun"),(1,"Ngapudaw"),(1,"Ngayokekaung"),(1,"Nyaungdon"),(1,"Pantanaw"),(1,"Chaung Thar"),(1,"Ngwesaung"),(1,"Pathein"),(1,"Shwethaungyan"),(1,"Ahmar"),(1,"Pyapon"),(1,"Thabaung"),(1,"Kyonmangae"),(1,"Wakema"),(1,"Ahthoke"),(1,"Ngathaingchaung"),(1,"Yegyi"),(1,"Zalun");

INSERT INTO tbl_home_township (cid,township_name) VALUES 
(2,"Bago"),(2,"Daik-U"),(2,"Gyobingauk"),(2,"Hpa Do"),(2,"Hpayargyi"),(2,"Hswar"),(2,"Htantabin"),(2,"Inn Ma"),(2,"Inntakaw"),(2,"Kanyutkwin"),(2,"Kawa"),(2,"Kaytumati"),(2,"Kyaukkyi"),(2,"Kyauktaga"),(2,"Kywe Pwe"),(2,"Letpadan"),(2,"Madauk"),(2,"Minhla"),(2,"Monyo"),(2,"Myo Hla"),(2,"Nattalin"),(2,"Nyaunglebin"),(2,"Oakshitpin"),(2,"Oe Thei Kone"),(2,"Okpho"),(2,"Oktwin"),(2,"Padaung"),(2,"Paukkhaung"),(2,"Paungdale"),(2,"Paungde"),(2,"Peinzalok"),(2,"Penwegon"),(2,"Phyu"),(2,"Puteekone"),(2,"Pyay"),(2,"Pyuntasa"),(2,"Shwedaung"),(2,"Shwegyin"),(2,"Sin Mee Swea"),(2,"Sit Kwin"),(2,"Tar Pun"),(2,"Taungoo"),(2,"Thanatpin"),(2,"Thayarwady"),(2,"Thegon"),(2,"Thetkala"),(2,"Thonse"),(2,"Waw"),(2,"Yae Ni"),(2,"Yedashe"),(2,"Zayyawadi"),(2,"Zigon");

INSERT INTO tbl_home_township (cid,township_name) VALUES 
(3,"Falam"),(3,"Rihkhawdar"),(3,"Webula"),(3,"Hakha"),(3,"Surkhua"),(3,"Kanpetlet"),(3,"Kyin Dway"),(3,"Lalengpi"),(3,"Matupi"),(3,"Rezua"),(3,"Mindat"),(3,"M'kuiimnu"),(3,"Paletwa"),(3,"Samee"),(3,"Khaikam"),(3,"Tedim"),(3,"Hnaring"),(3,"Thantlang"),(3,"Cikha"),(3,"Tonzang");

INSERT INTO tbl_home_township (cid,township_name) VALUES 
(4,"Bhamo"),(4,"Chipwi"),(4,"Pang War"),(4,"Hpakant"),(4,"Kamaing"),(4,"Injangyang"),(4,"Khaunglanhpu"),(4,"Machanbaw"),(4,"Mansi"),(4,"Mogaung"),(4,"Nammatee"),(4,"Hopin"),(4,"Inn Taw Gyi"),(4,"Mohnyin"),(4,"Nam Mun"),(4,"Dawthponeyan"),(4,"Lwegel"),(4,"Momauk"),(4,"Myitkyina"),(4,"Sinbo"),(4,"Nawngmun"),(4,"Pannandin"),(4,"Puta-O"),(4,"Myo Hla"),(4,"Shwegu"),(4,"Sumprabum"),(4,"Shin Bway Yang"),(4,"Tanai"),(4,"Tsawlaw"),(4,"Kan Paik Ti"),(4,"Sadung"),(4,"Waingmaw"),(4,"Nam Mar");

INSERT INTO tbl_home_township (cid,township_name) VALUES 
(5,"Bawlake"),(5,"Ywarthit"),(5,"Demoso"),(5,"Nan Mei Khon"),(5,"Hpasawng"),(5,"Hpruso"),(5,"Law Pi Ta"),(5,"Loikaw"),(5,"Loilen Lay"),(5,"Mese"),(5,"Shadaw");

INSERT INTO tbl_home_township (cid,township_name) VALUES 
(6,"Hlaingbwe"),(6,"Paingkyon"),(6,"Shan Ywar Thit"),(6,"Hpa-An"),(6,"Hpapun"),(6,"Kamarmaung"),(6,"Kawkareik"),(6,"Kyondoe"),(6,"Hpayarthonesu"),(6,"Kyaikdon"),(6,"Kyainseikgyi"),(6,"Myawaddy"),(6,"Su Ka Li"),(6,"Waw Lay Myaing (Waw Lay)"),(6,"Baw Ga Li"),(6,"Leik Tho"),(6,"Thandaung"),(6,"Thandaunggyi");


INSERT INTO tbl_home_township (cid,township_name) VALUES 
(7,"Aunglan"),(7,"Chauk"),(7,"Sa Lay"),(7,"Gangaw"),(7,"Kyaw"),(7,"Kamma"),(7,"Magway"),(7,"Minbu"),(7,"Saku"),(7,"Mindon"),(7,"Minhla"),(7,"Myaing"),(7,"Myothit"),(7,"Natmauk"),(7,"Ngape"),(7,"Kamma"),(7,"Myit Chay"),(7,"Pakokku"),(7,"Pauk"),(7,"Pwintbyu"),(7,"Salin"),(7,"Sinphyukyun"),(7,"Kyaukhtu"),(7,"Saw"),(7,"Seikphyu"),(7,"Sidoktaya"),(7,"Sinbaungwe"),(7,"Taungdwingyi"),(7,"Thayet"),(7,"Tilin"),(7,"Yenangyaung"),(7,"Yesagyo");

INSERT INTO tbl_home_township (cid,township_name) VALUES 
(8,"Amarapura"),(8,"Myitnge"),(8,"Aungmyaythazan"),(8,"Mandalay City"),(8,"Chanayethazan"),(8,"Chanmyathazi"),(8,"Kyaukpadaung"),(8,"Kyaukse"),(8,"Madaya"),(8,"Mahaaungmyay"),(8,"Mahlaing"),(8,"Meiktila"),(8,"Mogoke"),(8,"Myingyan"),(8,"Si Mee Khon"),(8,"Ku Me"),(8,"Myittha"),(8,"Natogyi"),(8,"Ngazun"),(8,"Bagan"),(8,"Ngathayauk"),(8,"Nyaung-U"),(8,"Patheingyi"),(8,"Pyawbwe"),(8,"Pyigyitagon"),(8,"Pyinoolwin"),(8,"Singu"),(8,"Sintgaing"),(8,"Tada-U"),(8,"Taungtha"),(8,"Takaung"),(8,"Thabeikkyin"),(8,"Thazi"),(8,"Wundwin"),(8,"Yamethin");

INSERT INTO tbl_home_township (cid,township_name) VALUES 
(9,"Bilin"),(9,"Chaungzon"),(9,"Kyaikmaraw"),(9,"Kyaikto"),(9,"Thein Za Yat"),(9,"Mawlamyine"),(9,"Kamarwet"),(9,"Mudon"),(9,"Paung"),(9,"Zinkyaik"),(9,"Kyaikkhami"),(9,"Thanbyuzayat"),(9,"Thaton"),(9,"Thuwunnawady"),(9,"Khawzar"),(9,"Lamaing"),(9,"Ye");

INSERT INTO tbl_home_township (cid,township_name) VALUES 
(10,"Det Khi Na Thi Ri"),(10,"Lewe"),(10,"Oke Ta Ra Thi Ri"),(10,"Poke Ba Thi RiTown"),(10,"Pyinmana"),(10,"Tatkon"),(10,"Za Bu Thi Ri"),(10,"Nay Pyi Taw"),(10,"Zay Yar Thi Ri");

INSERT INTO tbl_home_township (cid,township_name) VALUES 
(11,"Ann"),(11,"Tat Taung"),(11,"Buthidaung"),(11,"Gwa"),(11,"Kyeintali"),(11,"Kyaukpyu"),(11,"Sa Ne"),(11,"Kyauktaw"),(11,"Kha Maung Seik"),(11,"Maungdaw"),(11,"Myin Hlut"),(11,"Taungpyoletwea"),(11,"Minbya"),(11,"Mrauk-U"),(11,"Munaung"),(11,"Kanhtauntkyi"),(11,"Myebon"),(11,"Pauktaw"),(11,"Ponnagyun"),(11,"Lay Taung"),(11,"Ramree"),(11,"Rathedaung"),(11,"Sittwe"),(11,"Ngapali"),(11,"Thandwe"),(11,"Ma-Ei"),(11,"Tan Hlwe Ywar Ma"),(11,"Toungup");

INSERT INTO tbl_home_township (cid,township_name) VALUES 
(12,"Ayadaw"),(12,"Banmauk"),(12,"Budalin"),(12,"Chaung-U"),(12,"Hkamti"),(12,"Homalin"),(12,"Mo Waing Lut"),(12,"Shwe Pyi Aye"),(12,"Indaw"),(12,"Maw Lu"),(12,"Kale"),(12,"Kalewa"),(12,"Kanbalu"),(12,"Zee Kone"),(12,"Kani"),(12,"Katha"),(12,"Kawlin"),(12,"Khin-U"),(12,"Kyunhla"),(12,"Htan Par Kway"),(12,"Lahe"),(12,"Lay Shi"),(12,"Mo Paing Lut"),(12,"Sum Ma Rar"),(12,"Mawlaik"),(12,"Mingin"),(12,"Monywa"),(12,"Myaung"),(12,"Myinmu"),(12,"Don Hee"),(12,"Nanyun"),(12,"Pansaung"),(12,"Pale"),(12,"Paungbyin"),(12,"Pinlebu"),(12,"Sagaing"),(12,"Sar Taung"),(12,"Salingyi"),(12,"Kyauk Myaung"),(12,"Shwebo"),(12,"Saing Pyin"),(12,"Tabayin"),(12,"Khampat"),(12,"Myothit"),(12,"Tamu"),(12,"Taze"),(12,"Tigyaing"),(12,"Wetlet"),(12,"Wuntho"),(12,"Ye-U"),(12,"Yinmarbin");

INSERT INTO tbl_home_township (cid,township_name) VALUES 
(13,"Ho Tawng (Ho Tao)"),(13,"Kengtung"),(13,"Mong Hpen"),(13,"Mong Kar"),(13,"Mong Pawk"),(13,"Monghpyak"),(13,"Monghsat"),(13,"Mongkhoke"),(13,"Mongkhet"),(13,"Mongla"),(13,"Mongping"),(13,"Tontar"),(13,"Hmone Hta"),(13,"Mongton"),(13,"Ponparkyin"),(13,"Mongpauk"),(13,"Mongyang"),(13,"Mongyawng"),(13,"Mongyu"),(13,"Nam Hpai"),(13,"Kenglat"),(13,"Tachileik"),(13,"Tarlay"),(13,"Aik Chan (Ai' Chun)"),(13,"Hkun Mar (Hkwin Ma)"),(13,"Hopang"),(13,"Namtit"),(13,"Pan Lon"),(13,"Hsawng Hpa (Saun Pha)"),(13,"Hseni"),(13,"Hsipaw"),(13,"Ka Lawng Hpar"),(13,"Mong Maw"),(13,"Konkyan"),(13,"Maw Hteik"),(13,"Kunlong"),(13,"Kutkai"),(13,"Tarmoenye"),(13,"Kyaukme"),(13,"Monglon"),(13,"Mongngawt"),(13,"Lashio"),(13,"Chinshwehaw"),(13,"Laukkaing"),(13,"Lin Haw"),(13,"Long Htan"),(13,"Mabein"),(13,"Man Man Hseng"),(13,"Man Tun"),(13,"Manton"),(13,"Matman"),(13,"Mongmao"),(13,"Mongmit"),(13,"Mongyai"),(13,"Manhlyoe (Manhero)"),(13,"Monekoe"),(13,"Muse"),(13,"Pang Hseng (Kyu Koke)"),(13,"Nam Hkam Wu"),(13,"Nam Tit"),(13,"Namhkan"),(13,"Namhsan"),(13,"Namtu"),(13,"Nar Kawng"),(13,"Nar Wee (Na Wi)"),(13,"Narphan"),(13,"Nawng Hkit"),(13,"Wein Kawn"),(13,"Nawnghkio"),(13,"Pang Hkam"),(13,"Pang Yang"),(13,"Man Kan"),(13,"Pangsang"),(13,"Pangwaun"),(13,"Tangyan"),(13,"Yawng Lin"),(13,"Yin Pang"),(13,"Hopong"),(13,"Hsihseng"),(13,"Aungpan"),(13,"He Hoe"),(13,"Kalaw"),(13,"Kar Li"),(13,"Kunhing"),(13,"Kyethi"),(13,"Mongnawng"),(13,"Laihka"),(13,"Homein"),(13,"Langkho"),(13,"Intaw"),(13,"Lawksawk"),(13,"Loilen"),(13,"Pinlon"),(13,"Mawkmai"),(13,"Monghsu"),(13,"Mongsan (Hmonesan)"),(13,"Mongkaing"),(13,"Kengtawng"),(13,"Mongnai"),(13,"Mongpan"),(13,"Kho Lam"),(13,"Nansang"),(13,"Nang Pang"),(13,"Nyaungshwe"),(13,"Pekon"),(13,"Pindaya"),(13,"Naungtayar"),(13,"Pawng Lawng"),(13,"Pinlaung"),(13,"Ayetharyar"),(13,"Kyauktalonegyi"),(13,"Shwenyaung"),(13,"Taunggyi"),(13,"Ywangan");

INSERT INTO tbl_home_township (cid,township_name) VALUES 
(14,"Bokpyin"),(14,"Karathuri"),(14,"Pyigyimandaing"),(14,"Dawei"),(14,"Myitta"),(14,"Kawthoung"),(14,"Khamaukgyi"),(14,"Kyunsu"),(14,"Launglon"),(14,"Myeik"),(14,"Pala"),(14,"Palauk"),(14,"Palaw"),(14,"Maw Taung"),(14,"Tanintharyi"),(14,"Thayetchaung"),(14,"Kaleinaung"),(14,"Yebyu");


INSERT INTO tbl_home_township (cid,township_name) VALUES 
(15,"Ahlone"),(15,"Bahan"),(15,"Botahtaung"),(15,"Cocokyun"),(15,"Dagon"),(15,"Dagon Myothit (East)"),(15,"Dagon Myothit (North)"),(15,"Dagon Myothit (Seikkan)"),(15,"Dagon Myothit (South)"),(15,"Dala"),(15,"Dawbon"),(15,"Hlaing"),(15,"Hlaingtharya"),(15,"Hlegu"),(15,"Hmawbi"),(15,"Htantabin"),(15,"Insein"),(15,"Kamaryut"),(15,"Kawhmu"),(15,"Kayan"),(15,"Kungyangon"),(15,"Kyauktada"),(15,"Yangon City"),(15,"Kyauktan"),(15,"Tadar"),(15,"Kyeemyindaing"),(15,"Lanmadaw"),(15,"Latha"),(15,"Mayangone"),(15,"Htaukkyant"),(15,"Mingaladon"),(15,"Mingalartaungnyunt"),(15,"North Okkalapa"),(15,"Pabedan"),(15,"Pazundaung"),(15,"Sanchaung"),(15,"Seikgyikanaungto"),(15,"Seikkan"),(15,"Shwepyithar"),(15,"South Okkalapa"),(15,"Ahpyauk"),(15,"Okekan"),(15,"Taikkyi"),(15,"Tamwe"),(15,"Thaketa"),(15,"Thanlyin"),(15,"Thingangyun"),(15,"Thongwa"),(15,"Twantay"),(15,"Yankin");