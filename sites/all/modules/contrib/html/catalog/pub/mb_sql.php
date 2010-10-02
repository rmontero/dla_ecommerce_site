<?php 

function main() {
  show_header();
  show_form();
  show_footer();
}

function insert_tables() {
  $db_link = mysql_connect($_POST['db_server'], $_POST['db_user'], $_POST['db_passw']);
  if (!$db_link) {
    die("Can not connect to the server: ".mysql_error());
  }
  $db_selected = mysql_select_db($_POST['db_name'], $db_link);
  if (!$db_selected) {
    die("Can not select database: ".mysql_error());
  }
  foreach (query_text() as $query) {
    $db_result = mysql_query($query, $db_link);
    if (!$db_result) {
      die("Query failed: ".mysql_error());
    }
  }
  mysql_close($db_link);
  echo "Tables were successfully installed. You can go to the admin pages to set up the payment module.";
}



switch ($_POST['op']) {

  case "process":
    insert_tables();
    break;
    
  default:
    main();
    break;
}

function show_header() {
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
 "http://www.w3.org/TR/html4/loose.dtd">

<html>
<head>
  <title>osCommerce - Moneybookers module installation</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <style type="text/css">
<!-- 
    table {
      padding-left: 20px;
      text-align: right;
    }
    td {
      padding-top: 10px;
      padding-left: 5px;
    }
    th {
      width: 200px;
    }
    span.redtext {
      color: red;
    }
-->
  </style>
</head>
<body>
<?php
}

function show_footer() {
?>
</body>
</html>
<?php
}

function show_form() {
?>
<form action="mb_sql.php" method="post">
<table border="0" cellpadding="0" cellspacing="0" summary="database data">
<th colspan="2">
Please enter the following data to install the tables required for Moneybookers.com payment module
<br />
<span class="redtext">Do not forget to backup your database before proceeding with the installation!</span>
</th>
<tr>
  <td>Server:</td>
  <td><input name="db_server" type="text" /><br /></td>
</tr>
<tr>
  <td>Database:</td>
  <td><input name="db_name" type="text" /><br /></td>
</tr>
<tr>
  <td>User:</td>
  <td><input name="db_user" type="text" /><br /></td>
</tr>
<tr>
  <td>Password:</td>
  <td><input name="db_passw" type="text" /><br /></td>
</tr>
<tr colspan="2">
  <td><input type="submit" /></td>
</tr>
</table>
<input type="hidden" name="op" value="process" />
</form>
<?php
}

function query_text() {

$query[0] = "
DROP TABLE IF EXISTS payment_moneybookers, payment_moneybookers_countries, payment_moneybookers_currencies
";

$query[1] = "
CREATE TABLE payment_moneybookers (
  mb_TRID varchar(255) NOT NULL default '',
  mb_ERRNO smallint(3) unsigned NOT NULL default '0',
  mb_ERRTXT varchar(255) NOT NULL default '',
  mb_DATE datetime NOT NULL default '0000-00-00 00:00:00',
  mb_MBTID bigint(18) unsigned NOT NULL default '0',
  mb_STATUS tinyint(1) NOT NULL default '0',
  mb_ORDERID int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (mb_TRID)
) TYPE=MyISAM COMMENT='Moneybookers Transaction Table'
";
$query[2] = "
CREATE TABLE payment_moneybookers_countries (
  osc_cID int(11) NOT NULL default '0',
  mb_cID char(3) NOT NULL default '',
  PRIMARY KEY  (osc_cID)
) TYPE=MyISAM COMMENT='Moneybookers Accepted Countries'
";
$query[3] = "
INSERT INTO payment_moneybookers_countries (osc_cID, mb_cID) VALUES
    (2, 'ALB'),
    (3, 'ALG'),
    (4, 'AME'),
    (5, 'AND'),
    (6, 'AGL'),
    (7, 'ANG'),
    (9, 'ANT'),
    (10, 'ARG'),
    (11, 'ARM'),
    (12, 'ARU'),
    (13, 'AUS'),
    (14, 'AUT'),
    (15, 'AZE'),
    (16, 'BMS'),
    (17, 'BAH'),
    (18, 'BAN'),
    (19, 'BAR'),
    (20, 'BLR'),
    (21, 'BGM'),
    (22, 'BEL'),
    (23, 'BEN'),
    (24, 'BER'),
    (26, 'BOL'),
    (27, 'BOS'),
    (28, 'BOT'),
    (30, 'BRA'),
    (32, 'BRU'),
    (33, 'BUL'),
    (34, 'BKF'),
    (35, 'BUR'),
    (36, 'CAM'),
    (37, 'CMR'),
    (38, 'CAN'),
    (39, 'CAP'),
    (40, 'CAY'),
    (41, 'CEN'),
    (42, 'CHA'),
    (43, 'CHL'),
    (44, 'CHN'),
    (47, 'COL'),
    (49, 'CON'),
    (51, 'COS'),
    (52, 'COT'),
    (53, 'CRO'),
    (54, 'CUB'),
    (55, 'CYP'),
    (56, 'CZE'),
    (57, 'DEN'),
    (58, 'DJI'),
    (59, 'DOM'),
    (60, 'DRP'),
    (62, 'ECU'),
    (64, 'EL_'),
    (65, 'EQU'),
    (66, 'ERI'),
    (67, 'EST'),
    (68, 'ETH'),
    (70, 'FAR'),
    (71, 'FIJ'),
    (72, 'FIN'),
    (73, 'FRA'),
    (75, 'FRE'),
    (78, 'GAB'),
    (79, 'GAM'),
    (80, 'GEO'),
    (81, 'GER'),
    (82, 'GHA'),
    (83, 'GIB'),
    (84, 'GRC'),
    (85, 'GRL'),
    (87, 'GDL'),
    (88, 'GUM'),
    (89, 'GUA'),
    (90, 'GUI'),
    (91, 'GBS'),
    (92, 'GUY'),
    (93, 'HAI'),
    (95, 'HON'),
    (96, 'HKG'),
    (97, 'HUN'),
    (98, 'ICE'),
    (99, 'IND'),
    (101, 'IRN'),
    (102, 'IRA'),
    (103, 'IRE'),
    (104, 'ISR'),
    (105, 'ITA'),
    (106, 'JAM'),
    (107, 'JAP'),
    (108, 'JOR'),
    (109, 'KAZ'),
    (110, 'KEN'),
    (112, 'SKO'),
    (113, 'KOR'),
    (114, 'KUW'),
    (115, 'KYR'),
    (116, 'LAO'),
    (117, 'LAT'),
    (141, 'MCO'),
    (119, 'LES'),
    (120, 'LIB'),
    (121, 'LBY'),
    (122, 'LIE'),
    (123, 'LIT'),
    (124, 'LUX'),
    (125, 'MAC'),
    (126, 'F.Y'),
    (127, 'MAD'),
    (128, 'MLW'),
    (129, 'MLS'),
    (130, 'MAL'),
    (131, 'MLI'),
    (132, 'MLT'),
    (134, 'MAR'),
    (135, 'MRT'),
    (136, 'MAU'),
    (138, 'MEX'),
    (140, 'MOL'),
    (142, 'MON'),
    (143, 'MTT'),
    (144, 'MOR'),
    (145, 'MOZ'),
    (76, 'PYF'),
    (147, 'NAM'),
    (149, 'NEP'),
    (150, 'NED'),
    (151, 'NET'),
    (152, 'CDN'),
    (153, 'NEW'),
    (154, 'NIC'),
    (155, 'NIG'),
    (69, 'FLK'),
    (160, 'NWY'),
    (161, 'OMA'),
    (162, 'PAK'),
    (164, 'PAN'),
    (165, 'PAP'),
    (166, 'PAR'),
    (167, 'PER'),
    (168, 'PHI'),
    (170, 'POL'),
    (171, 'POR'),
    (172, 'PUE'),
    (173, 'QAT'),
    (175, 'ROM'),
    (176, 'RUS'),
    (177, 'RWA'),
    (178, 'SKN'),
    (179, 'SLU'),
    (180, 'ST.'),
    (181, 'WES'),
    (182, 'SAN'),
    (183, 'SAO'),
    (184, 'SAU'),
    (185, 'SEN'),
    (186, 'SEY'),
    (187, 'SIE'),
    (188, 'SIN'),
    (189, 'SLO'),
    (190, 'SLV'),
    (191, 'SOL'),
    (192, 'SOM'),
    (193, 'SOU'),
    (195, 'SPA'),
    (196, 'SRI'),
    (199, 'SUD'),
    (200, 'SUR'),
    (202, 'SWA'),
    (203, 'SWE'),
    (204, 'SWI'),
    (205, 'SYR'),
    (206, 'TWN'),
    (207, 'TAJ'),
    (208, 'TAN'),
    (209, 'THA'),
    (210, 'TOG'),
    (212, 'TON'),
    (213, 'TRI'),
    (214, 'TUN'),
    (215, 'TUR'),
    (216, 'TKM'),
    (217, 'TCI'),
    (219, 'UGA'),
    (231, 'BRI'),
    (221, 'UAE'),
    (222, 'GBR'),
    (223, 'UNI'),
    (225, 'URU'),
    (226, 'UZB'),
    (227, 'VAN'),
    (229, 'VEN'),
    (230, 'VIE'),
    (232, 'US_'),
    (235, 'YEM'),
    (236, 'YUG'),
    (238, 'ZAM'),
    (239, 'ZIM')
";
$query[4] = "
CREATE TABLE payment_moneybookers_currencies (
  mb_currID char(3) NOT NULL default '',
  mb_currName varchar(255) NOT NULL default '',
  PRIMARY KEY  (mb_currID)
) TYPE=MyISAM COMMENT='Moneybookers Accepted Currencies'
";
$query[5] = "
INSERT INTO payment_moneybookers_currencies (mb_currID, mb_currName) VALUES 
  ('AUD', 'Australian Dollar'),
  ('GBP', 'Brithis Pound'),
  ('BGN', 'Bulgarian Leva'),
  ('CAD', 'Canadian Dollar'),
  ('CZK', 'Czech Koruna'),
  ('DKK', 'Danish Krone'),
  ('EEK', 'Estonian Koruna'),
  ('EUR', 'Euro'),
  ('HKD', 'Hong Kong Dollar'),
  ('HUF', 'Forint'),
  ('ISK', 'Iceland Krona'),
  ('INR', 'Indian Rupee'),
  ('ILS', 'Israeli Shekel'),
  ('JPY', 'Japanese Yen'),
  ('LVL', 'Latvian Lat'),
  ('MYR', 'Malaysian Ringgit'),
  ('NZD', 'New Zealand Dollar'),
  ('NOK', 'Norwegian Krone'),
  ('PLN', 'Polish Zloty'),
  ('ROL', 'Romanian Leu'),
  ('SGD', 'Singapore Dollar'),
  ('SKK', 'Slovakian Koruna'),
  ('SIT', 'Slovenian Tollar'),
  ('ZAR', 'South-African Rand'),
  ('KRW', 'South-Korean Won'),
  ('SEK', 'Swedish Krona'),
  ('CHF', 'Swiss Franc'),
  ('TWD', 'Taiwan Dollar'),
  ('THB', 'Thailand Baht'),
  ('USD', 'US Dollar')
";

return $query;
}
?>