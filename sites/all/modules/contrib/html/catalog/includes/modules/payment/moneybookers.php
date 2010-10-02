<?php
/*
  $Id: moneybookers.php,v 1.2 2005/06/18 19:14:00  MaG Exp $

  osCommerce, Open Source E-Commerce Solutions
  http://www.oscommerce.com

  Copyright (c) 2005 osCommerce

  Released under the GNU General Public License
*/

  class moneybookers {
    var $code, $title, $description, $sort_order, $enabled = false, $form_action_url;
    var $email_footer, $auth_num, $transaction_id, $mbLanguages, $mbCurrencies, $aCurrencies, $defCurr, $defLang;

    // class constructor
    function moneybookers() {
      global $osC_Database;

      $this->code = 'moneybookers';
      $this->title = MODULE_PAYMENT_MONEYBOOKERS_TEXT_TITLE;
      $this->description = MODULE_PAYMENT_MONEYBOOKERS_TEXT_DESCRIPTION;

      // text to include in the e-mail moneybookers.com sends during the
      // transaction
      $this->email_footer = MODULE_PAYMENT_MONEYBOOKERS_TEXT_EMAIL_FOOTER;

      $this->auth_num = '';
      $this->transaction_id = '';

      // languages moneybookers supports on its payment interface      
      $this->mbLanguages = array("EN", "DE", "ES", "FR", "IT", "PL");

      $Qselect = $osC_Database->query("SELECT mb_currID FROM payment_moneybookers_currencies");
      $Qselect->execute();
      while ($Qselect->next()) {
        $this->mbCurrencies[] = $Qselect->value('mb_currID');
      }
      $Qselect->freeResult();

      $Qselect = $osC_Database->query("SELECT code FROM :table_currencies");
      $Qselect->bindTable(':table_currencies', TABLE_CURRENCIES);
      $Qselect->execute();
      while ($Qselect->next()) {
        $this->aCurrencies[] = $Qselect->value('code');
      }
      $Qselect->freeResult();

      if (defined('MODULE_PAYMENT_MONEYBOOKERS_STATUS')) {
        $this->initialize();
      }

    }
    
    function initialize() {
      global $osC_Database, $order;      

      $this->sort_order = MODULE_PAYMENT_MONEYBOOKERS_SORT_ORDER;
      $this->enabled = ((MODULE_PAYMENT_MONEYBOOKERS_STATUS == 'True') ? true : false);
      // take the default currency and language to be used at
      // moneybookers.com
      $Qselect = $osC_Database->query("SELECT configuration_value FROM :table_configuration WHERE configuration_key = 'DEFAULT_CURRENCY'");
      $Qselect->bindTable(':table_configuration', TABLE_CONFIGURATION);
      $Qselect->execute();
      $Qselect->next();
      $this->defCurr = $Qselect->value('configuration_value');
      $Qselect->freeResult();

      $Qselect = $osC_Database->query("SELECT configuration_value FROM :table_configuration WHERE configuration_key = 'DEFAULT_LANGUAGE'");
      $Qselect->bindTable(':table_configuration', TABLE_CONFIGURATION);
      $Qselect->execute();
      $Qselect->next();
      $this->defLang = $Qselect->value('configuration_value');
      $Qselect->freeResult();

      // If the default language isn't supported by moneybookers.com
      // fall back to English: "EN"
      $this->defLang = strtoupper($this->defLang);
      if (!in_array($this->defLang, $this->mbLanguages)) {
        $this->defLang = "EN";
      }

      if ((int)MODULE_PAYMENT_MONEYBOOKERS_ORDER_STATUS_ID > 0) {
        $this->order_status = MODULE_PAYMENT_MONEYBOOKERS_ORDER_STATUS_ID;
      }

      if (is_object($order)) $this->update_status();

      $this->form_action_url = 'https://www.moneybookers.com/app/payment.pl';
    }

////
// Status update
    function update_status() {
      global $osC_Database, $order;

      if ( ($this->enabled == true) && ((int)MODULE_PAYMENT_MONEYBOOKERS_ZONE > 0) ) {
        $check_flag = false;

        $Qcheck = $osC_Database->query('select zone_id from :table_zones_to_geo_zones where geo_zone_id = :geo_zone_id and zone_country_id = :zone_country_id order by zone_id');
        $Qcheck->bindTable(':table_zones_to_geo_zones', TABLE_ZONES_TO_GEO_ZONES);
        $Qcheck->bindInt(':geo_zone_id', MODULE_PAYMENT_MONEYBOOKERS_ZONE);
        $Qcheck->bindInt(':zone_country_id', $order->billing['country']['id']);
        $Qcheck->execute();

        while ($Qcheck->next()) {
          if ($Qcheck->valueInt('zone_id') < 1) {
            $check_flag = true;
            break;
          } elseif ($Qcheck->valueInt('zone_id') == $order->billing['zone_id']) {
            $check_flag = true;
            break;
          }
        }

        if ($check_flag == false) {
          $this->enabled = false;
        }
      }
    }

// class methods
    function javascript_validation() {
      return false;
    }

    function selection() {
      return array('id' => $this->code,
                   'module' => $this->title);
    }

    function pre_confirmation_check() {
      return false;
    }

    function confirmation() {
      return false;
    }

    function process_button() {
      global $osC_Session, $order, $order_total_modules, $osC_Currencies, $osC_Database;

      $Qselect = $osC_Database->query("SELECT code FROM :table_languages WHERE languages_id = :language_id");
      $Qselect->bindTable(':table_languages', TABLE_LANGUAGES);
      $Qselect->bindInt(":language_id", $osC_Session->value('languages_id'));
      $Qselect->execute();
      $Qselect->next();
      $lang_code = $Qselect->value('code');
      $Qselect->freeResult();

      $mbLanguage = strtoupper($lang_code);

      // *
      // If you have "select default currency with language" set
      // you can create US english, and UK english language sets
      // separately in osC, and bind USD and GBP respectively to
      // those language sets. (ie. If a user select UK english as his/her
      // default language, the currency will also default to GBP.)
      // If you do not use this "feature", you can delete the following
      // 3 lines.
      
      // English is English, so set US english to "EN"
      if ($mbLanguage == "US") {
        $mbLanguage = "EN";
      }
      // *
      
      // if the user language is not supported by moneybookers.com
      // fall back to the default language
      // set on the admin interface of the module
      if (!in_array($mbLanguage, $this->mbLanguages)) {
        $mbLanguage = MODULE_PAYMENT_MONEYBOOKERS_LANGUAGE;
      }

      // if the user currency is not supported by moneybookers.com
      // fall back to the default currency
      // set on the admin interface of the module
      $mbCurrency = $osC_Session->value('currency');
      if (!in_array($osC_Session->value('currency'), $this->mbCurrencies)) {
        $mbCurrency = MODULE_PAYMENT_MONEYBOOKERS_CURRENCY;
      }

      $Qselect = $osC_Database->query("SELECT mb_cID FROM payment_moneybookers_countries, :table_countries WHERE (osc_cID = countries_id) AND (countries_id = :billing_country)");
      $Qselect->bindTable(':table_countries', TABLE_COUNTRIES);
      $Qselect->bindValue(":billing_country", $order->billing['country']['id']);
      $Qselect->execute();
      $Qselect->next();
      $mbCountry = $Qselect->value('mb_cID');
      $Qselect->freeResult();

      // generate a unique transaction ID for the process
      $this->transaction_id = $this->generate_trid();

      $Qinsert = $osC_Database->query("INSERT INTO payment_moneybookers (mb_TRID, mb_DATE) VALUES (:transaction_id, NOW())");
      $Qinsert->bindValue(":transaction_id", $this->transaction_id);
      $Qinsert->execute();
      $Qinsert->next();
      $Qinsert->freeResult();

      $process_button_string = osc_draw_hidden_field('pay_to_email', MODULE_PAYMENT_MONEYBOOKERS_EMAILID) .
                               osc_draw_hidden_field('transaction_id', $this->transaction_id) .
                               osc_draw_hidden_field('return_url', tep_href_link(FILENAME_CHECKOUT, 'process&trid='.$this->transaction_id)) .
                               osc_draw_hidden_field('cancel_url', tep_href_link(FILENAME_CHECKOUT, 'payment&'.MODULE_PAYMENT_MONEYBOOKERS_ERRORTEXT1.$this->code.MODULE_PAYMENT_MONEYBOOKERS_ERRORTEXT2)) .
                               osc_draw_hidden_field('status_url', 'mailto:' . MODULE_PAYMENT_MONEYBOOKERS_EMAILID) .
                               osc_draw_hidden_field('language', $mbLanguage) .
                               osc_draw_hidden_field('pay_from_email', $order->customer['email_address']) .
                               osc_draw_hidden_field('amount', tep_round($order->info['total'] * $osC_Currencies->value($mbCurrency), $osC_Currencies->decimalPlaces($mbCurrency))) .
                               osc_draw_hidden_field('currency', $mbCurrency) .
                               osc_draw_hidden_field('detail1_description', STORE_NAME) .
                               osc_draw_hidden_field('detail1_text', MODULE_PAYMENT_MONEYBOOKERS_ORDER_TEXT . strftime(DATE_FORMAT_LONG)) .
                               osc_draw_hidden_field('firstname', $order->billing['firstname']) .
                               osc_draw_hidden_field('lastname', $order->billing['lastname']) .
                               osc_draw_hidden_field('address', $order->billing['street_address']) .
                               osc_draw_hidden_field('postal_code', $order->billing['postcode']) .
                               osc_draw_hidden_field('city', $order->billing['city']) .
                               osc_draw_hidden_field('state', $order->billing['state']) .
                               osc_draw_hidden_field('country', $mbCountry) .
                               osc_draw_hidden_field('confirmation_note', MODULE_PAYMENT_MONEYBOOKERS_CONFIRMATION_TEXT);

      if (ereg("[0-9]{6}", MODULE_PAYMENT_MONEYBOOKERS_REFID)) {
        $process_button_string .= osc_draw_hidden_field('rid', MODULE_PAYMENT_MONEYBOOKERS_REFID);
      }

      // moneyboocers.com payment gateway does not accept accented characters!
      // Please feel free to add any other accented characters to the list.
      return strtr($process_button_string, "áéíóöõúüûÁÉÍÓÖÕÚÜÛ", "aeiooouuuAEIOOOUUU");
    }

    // manage returning data from moneybookers (errors, failures, success etc.)
    function before_process() {

      global $order, $osC_Database;

      // get the transaction ID
      if (isset($_GET['trid'])) {
        $this->transaction_id = $_GET["trid"];
      } else {
        $this->transaction_id = $_POST["transaction_id"];
      }
      // create password hash
      $md5_pwd = md5(MODULE_PAYMENT_MONEYBOOKERS_PWD);
      // build URL to retrieve transaction result
      $queryURL = "https://www.moneybookers.com/app/query.pl?email=".MODULE_PAYMENT_MONEYBOOKERS_EMAILID."&password=".$md5_pwd."&action=status_trn&trn_id=".$this->transaction_id;
//echo $queryURL; die();
      // query moneybookers.com for the result of our transaction
      $ch = curl_init($queryURL);
      curl_setopt ($ch, CURLOPT_HEADER, 0);
      curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
      $result = curl_exec($ch);
      curl_close($ch);
      
      $result = urldecode($result);

      /********************************/
      // get the returned error code
      // 200 -- OK
      // 401 -- Login failed 
      // 402 -- Unknown action
      // 403 -- Transaction not found
      // 404 -- Missing parameter
      // 405 -- Illegal parameter value
      /********************************/
      preg_match("/\d{3}/", $result, $return_code);

      switch ($return_code[0]) {


        // query was OK, data is sent back
        case "200":
          $result = strstr($result, "status");
          $aResult = explode("&", $result);
    
          /***********************************************************/
          // get the returned data
          // [status] -- (-2) => failed
          //             ( 2) => processed
          //             ( 1) => scheduled (eg. offline bank transfer)
          // [mb_transaction_id] -- transaction ID at moneybookers.com
          /***********************************************************/
          foreach ($aResult as $value) {
            list($parameter, $pVal) = explode("=", $value);
            $aFinal["$parameter"] = $pVal;
          }

          $Qupdate = $osC_Database->query("UPDATE payment_moneybookers SET mb_ERRNO = :mb_ERRNO, mb_ERRTXT = :mb_ERRTXT, mb_MBTID = :mb_MBTID, mb_STATUS = :mb_STATUS WHERE mb_TRID = :transaction_id");
          $Qupdate->bindValue(":mb_MBTID", $aFinal['mb_transaction_id']);
          $Qupdate->bindValue(":mb_STATUS", $aFinal['status']);
          $Qupdate->bindValue(":transaction_id", $this->transaction_id);
          if ($aFinal["status"] == -2) {
            // if there was an error, update the database according to it
            $Qupdate->bindValue(":mb_ERRNO", "999");
            $Qupdate->bindValue(":mb_ERRTXT", "Transaction failed.");
            $Qupdate->execute();
            $Qupdate->next();
            $Qupdate->freeResult();
            $payment_error_return = "ERROR-CODE:: -2, ERROR:: ".MODULE_PAYMENT_MONEYBOOKERS_TRANSACTION_FAILED_TEXT;
            tep_redirect(tep_href_link(FILENAME_CHECKOUT, 'payment&payment_error='.$this->code.'&error='.$payment_error_return, 'SSL', true, false));
            return false;
          } else {
            // if everything went good, update the data in the database and in the order class
            $Qupdate->bindValue(":mb_ERRNO", "200");
            $Qupdate->bindValue(":mb_ERRTXT", "OK");
            $Qupdate->execute();
            $Qupdate->next();
            $Qupdate->freeResult();
            $moneybookers_payment_comment = MODULE_PAYMENT_MONEYBOOKERS_ORDER_COMMENT1.$aFinal["mb_transaction_id"]." (".MODULE_PAYMENT_MONEYBOOKERS_ORDER_COMMENT2.") ";
            $order->info['comments'] = $moneybookers_payment_comment.$order->info['comments'];
          }

          break;


        // error occured during query
        // errors documented in the moneybookers doc
        case "401":
        case "402":
        case "403":
        case "404":
        case "405":
          preg_match("/[^\d\t]+.*/i", $result, $return_array);
          $Qupdate = $osC_Database->query("UPDATE payment_moneybookers SET mb_ERRNO = :mb_ERRNO, mb_ERRTXT = :mb_ERRTXT WHERE mb_TRID = :transaction_id");
          $Qupdate->bindValue(":mb_ERRNO", $return_code[0]);
          $Qupdate->bindValue(":mb_ERRTXT", $return_array[0]);
          $Qupdate->bindValue(":transaction_id", $this->transaction_id);
          $Qupdate->execute();
          $Qupdate->next();
          $Qupdate->freeResult();
          $payment_error_return = "ERROR_CODE:: {$return_code[0]}, ERROR:: {$return_array[0]}";
          tep_redirect(tep_href_link(FILENAME_CHECKOUT, 'payment&payment_error='.$this->code.'&error='.$payment_error_return, 'SSL', true, false));
          break;


        // unknown error
        default:
          $payment_error_return = "ERROR_CODE:: 000, ERROR:: Unknown error!";
          tep_redirect(tep_href_link(FILENAME_CHECKOUT, 'payment&payment_error='.$this->code.'&error='.$payment_error_return, 'SSL', true, false));
          break;

      }

    }

    function after_process() {
      global $osC_Database;
      // Finally, insert osCommerce order ID into the moneybookers table
      $Qupdate = $osC_Database->query("UPDATE payment_moneybookers SET mb_ORDERID = :mb_ORDERID WHERE mb_TRID = :transaction_id");
      $Qupdate->bindValue(":mb_ORDERID", $insert_id);
      $Qupdate->bindValue(":transaction_id", $this->transaction_id);
      $Qupdate->execute();
      $Qupdate->next();
      $Qupdate->freeResult();
    }

    function get_error() {

      $error = array('title' => MODULE_PAYMENT_MONEYBOOKERS_TEXT_ERROR,
                     'error' => stripslashes(urldecode($_GET['error'])));

      return $error;
    }

    function check() {
      if (!isset($this->_check)) {
        $this->_check = defined('MODULE_PAYMENT_MONEYBOOKERS_STATUS');
      }

      return $this->_check;
    }

    function install() {
      global $osC_Database;

      if (!$this->check_currency($this->aCurrencies)) {
        $this->enabled = false;
        $install_error_return = 'set=payment&module=moneybookers&error=' . MODULE_PAYMENT_MONEYBOOKERS_NOCURRENCY_ERROR;
        tep_redirect(tep_href_link(FILENAME_MODULES, $install_error_return, 'SSL', true, false));
        return false;
      }
      $osC_Database->simpleQuery("insert into " . TABLE_CONFIGURATION . " (configuration_title, configuration_key, configuration_value, configuration_description, configuration_group_id, sort_order, set_function, date_added) values ('Enable moneybookers Module', 'MODULE_PAYMENT_MONEYBOOKERS_STATUS', 'True', 'Do you want to accept moneybookers.com payments?', '6', '0', 'tep_cfg_select_option(array(\'True\', \'False\'), ', now())");
      $osC_Database->simpleQuery("insert into " . TABLE_CONFIGURATION . " (configuration_title, configuration_key, configuration_value, configuration_description, configuration_group_id, sort_order, date_added) values ('E-Mail Address', 'MODULE_PAYMENT_MONEYBOOKERS_EMAILID', '', 'Merchant\'s e-mail address registered at moneybookers.com', '6', '1', now())");
      $osC_Database->simpleQuery("insert into " . TABLE_CONFIGURATION . " (configuration_title, configuration_key, configuration_value, configuration_description, configuration_group_id, sort_order, date_added) values ('Moneybookers password', 'MODULE_PAYMENT_MONEYBOOKERS_PWD', '', 'Enter your moneybookers password (it is required for the transaction result query!)', '6', '2', now())");
      $osC_Database->simpleQuery("insert into " . TABLE_CONFIGURATION . " (configuration_title, configuration_key, configuration_value, configuration_description, configuration_group_id, sort_order, date_added) values ('Referral ID', 'MODULE_PAYMENT_MONEYBOOKERS_REFID', '', 'Your personal Referral ID from moneybookers.com', '6', '3', now())");
      $osC_Database->simpleQuery("insert into " . TABLE_CONFIGURATION . " (configuration_title, configuration_key, configuration_value, configuration_description, configuration_group_id, sort_order, date_added) values ('Sort order of display.', 'MODULE_PAYMENT_MONEYBOOKERS_SORT_ORDER', '0', 'Sort order of display. Lowest is displayed first.', '6', '4', now())");
      $osC_Database->simpleQuery("insert into " . TABLE_CONFIGURATION . " (configuration_title, configuration_key, configuration_value, configuration_description, configuration_group_id, sort_order, set_function, date_added) values ('Transaction Currency', 'MODULE_PAYMENT_MONEYBOOKERS_CURRENCY', '".$this->defCurr."', 'Select the default currency for the payment transactions. If the user selected currency is not available at moneybookers.com, this currency will be the payment currency.', '6', '5', 'tep_cfg_select_option(".$this->show_array($this->aCurrencies)."), ', now())");
      $osC_Database->simpleQuery("insert into " . TABLE_CONFIGURATION . " (configuration_title, configuration_key, configuration_value, configuration_description, configuration_group_id, sort_order, set_function, date_added) values ('Transaction Language', 'MODULE_PAYMENT_MONEYBOOKERS_LANGUAGE', '".$this->defLang."', 'Select the default language for the payment transactions. If the user selected language is not available at moneybookers.com, this currency will be the payment language.', '6', '6', 'tep_cfg_select_option(".$this->show_array($this->mbLanguages)."), ', now())");
      $osC_Database->simpleQuery("insert into " . TABLE_CONFIGURATION . " (configuration_title, configuration_key, configuration_value, configuration_description, configuration_group_id, sort_order, use_function, set_function, date_added) values ('Payment Zone', 'MODULE_PAYMENT_MONEYBOOKERS_ZONE', '0', 'If a zone is selected, only enable this payment method for that zone.', '6', '7', 'tep_get_zone_class_title', 'tep_cfg_pull_down_zone_classes(', now())");
      $osC_Database->simpleQuery("insert into " . TABLE_CONFIGURATION . " (configuration_title, configuration_key, configuration_value, configuration_description, configuration_group_id, sort_order, set_function, use_function, date_added) values ('Set Order Status', 'MODULE_PAYMENT_MONEYBOOKERS_ORDER_STATUS_ID', '0', 'Set the status of orders made with this payment module to this value', '6', '8', 'tep_cfg_pull_down_order_statuses(', 'tep_get_order_status_name', now())");
    }

    function remove() {
      global $osC_Database;

      $Qdel = $osC_Database->query('delete from :table_configuration where configuration_key in (":configuration_key")');
      $Qdel->bindTable(':table_configuration', TABLE_CONFIGURATION);
      $Qdel->bindRaw(':configuration_key', implode('", "', $this->keys()));
      $Qdel->execute();
    }

    function keys() {
      return array('MODULE_PAYMENT_MONEYBOOKERS_STATUS', 'MODULE_PAYMENT_MONEYBOOKERS_EMAILID', 'MODULE_PAYMENT_MONEYBOOKERS_PWD', 'MODULE_PAYMENT_MONEYBOOKERS_REFID', 'MODULE_PAYMENT_MONEYBOOKERS_LANGUAGE', 'MODULE_PAYMENT_MONEYBOOKERS_CURRENCY', 'MODULE_PAYMENT_MONEYBOOKERS_SORT_ORDER', 'MODULE_PAYMENT_MONEYBOOKERS_ORDER_STATUS_ID', 'MODULE_PAYMENT_MONEYBOOKERS_ZONE');
    }

    // If there is no moneybookers accepted currency configured with the shop
    // do not allow the moneybookers payment module installation
    function check_currency($availableCurr) {
      $foundCurr = false;
      foreach ($availableCurr as $currID) {
        if (in_array($currID, $this->mbCurrencies)) { 
          $foundCurr = true;
        }
      }
      return $foundCurr;
    }

    // Parse the predefinied array to be 'module install' friendly
    // as it is used for select in the module's install() function
    function show_array($aArray) {
      $aFormatted = "array(";
      foreach ($aArray as $key=>$sVal) {
        $aFormatted .= "\'$sVal\', ";
      }
      $aFormatted = substr($aFormatted, 0, strlen($aFormatted)-2);
      return $aFormatted;
    }

    // generate a unique 16 characters long transaction ID and
    // check against the already existing IDs in the databse
    // loop it until it's really unique
    function generate_trid() {
      global $osC_Database;

      do {
        $trid = tep_create_random_value(16, "mixed");
        $Qselect = $osC_Database->query("SELECT mb_TRID FROM payment_moneybookers WHERE mb_TRID = :mb_TRID");
        $Qselect->bindValue(":mb_TRID", $trid);
        $Qselect->execute();
      } while ($Qselect->numberOfRows());

      $Qselect->freeResult();
      return $trid;
    
    }
  }
?>