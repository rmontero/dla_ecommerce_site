<?php
/* $Id: scripturefilter.inc,v 1.3.4.1 2007/05/21 16:45:34 smsimms Exp $ */

define('DEFAULT_BIBLE_TRANSLATION','NIV');

function scripturize ($text = '',$bible = DEFAULT_BIBLE_TRANSLATION){
    // skip everything within a hyperlink, a <pre> block, a <code> block, or a tag
    // we skip tags because something like <img src="blah" alt="John 3:16"> should not be messed with
    $anchor_regex = '<a\s+href.*?<\/a>';
    $pre_regex = '<pre>.*<\/pre>';
    $code_regex = '<code>.*<\/code>';
    $tag_regex = '<(?:[^<>\s]*)(?:\s[^<>]*){0,1}>'; // $tag_regex='<[^>]+>';
    $split_regex = "/((?:$anchor_regex)|(?:$pre_regex)|(?:$code_regex)|(?:$tag_regex))/i";

    $parsed_text = preg_split($split_regex,$text,-1,PREG_SPLIT_DELIM_CAPTURE);
    $linked_text = '';

    while (list($key,$value) = each($parsed_text)) {
        if (preg_match($split_regex,$value)) {
	    $linked_text .= $value; // if it is an HTML element or within a link, just leave it as is
	} else {
            $linked_text .= scripturizeAddLinks($value,$bible); // if it's text, parse it for Bible references
	}
    }
    
    return $linked_text;
}

function scripturizeAddLinks($text = '',$bible = DEFAULT_BIBLE_TRANSLATION) {
    $volume_regex = '1|2|3|I|II|III|1st|2nd|3rd|First|Second|Third';

    $book_regex  = 'Genesis|Exodus|Leviticus|Numbers|Deuteronomy|Joshua|Judges|Ruth|Samuel|Kings|Chronicles|Ezra|Nehemiah|Esther';
    $book_regex .= '|Job|Psalms?|Proverbs?|Ecclesiastes|Songs? of Solomon|Song of Songs|Isaiah|Jeremiah|Lamentations|Ezekiel|Daniel|Hosea|Joel|Amos|Obadiah|Jonah|Micah|Nahum|Habakkuk|Zephaniah|Haggai|Zechariah|Malachi';
    $book_regex .= '|Mat+hew|Mark|Luke|John|Acts?|Acts of the Apostles|Romans|Corinthians|Galatians|Ephesians|Phil+ippians|Colossians|Thessalonians|Timothy|Titus|Philemon|Hebrews|James|Peter|Jude|Revelations?';

    // split these up from the Perl code because I want to be able to have an optional period at the end of just the abbreviations

    $abbrev_regex  = 'Gen|Ex|Exo|Lev|Num|Nmb|Deut?|Josh?|Judg?|Jdg|Rut|Sam|Ki?n|Chr(?:on?)?|Ezr|Neh|Est';
    $abbrev_regex .= '|Jb|Psa?|Pr(?:ov?)?|Eccl?|Song?|Isa|Jer|Lam|Eze|Dan|Hos|Joe|Amo|Oba|Jon|Mic|Nah|Hab|Zeph?|Hag|Zech?|Mal';
    $abbrev_regex .= '|Mat|Mr?k|Lu?k|Jh?n|Jo|Act|Rom|Cor|Gal|Eph|Col|Phi|The?|Thess?|Tim|Tit|Phile|Heb|Ja?m|Pe?t|Ju?d|Rev';

    $book_regex='(?:'.$book_regex.')|(?:'.$abbrev_regex.')\.?';

    $verse_regex="\d{1,3}(?::\d{1,3})?(?:\s?(?:[-&,]\s?\d+))*";

    $translation_regex = 'NIV|NASB|AMP|NLT|KJV|ESV|CEV|NET|NKJV|KJ21|ASV|WE|YLT|DARBY|WYC|NIV-UK|TNIV|MSG|NIRV';

    // note that this will be executed as PHP code after substitution thanks to the /e at the end!

    $passage_regex = '/(?:('.$volume_regex.')\s)?('.$book_regex.')\s('.$verse_regex.')(?:\s?[,-]?\s?((?:'.$translation_regex.')|\s?\((?:'.$translation_regex.')\)))?/e';

    $replacement_regex = "scripturizeLinkReference('\\0','\\1','\\2','\\3','\\4','$bible')";

    $text=preg_replace($passage_regex,$replacement_regex,$text);

    return $text;
}

function scripturizeLinkReference($reference='',$volume='',$book='',$verse='',$translation='',$user_translation='') {
    if ($volume) {
       $volume = str_replace('III','3',$volume);
       $volume = str_replace('II','2',$volume);
       $volume = str_replace('I','1',$volume);
       $volume = $volume{0}; // will remove st,nd,and rd (presupposes regex is correct)
    }

   if(!$translation) {
         if (!$user_translation) {
             $translation = DEFAULT_BIBLE_TRANSLATION;
         } else {
             $translation = $user_translation;
         }
   } else {
       $translation = trim($translation,' ()'); // strip out any parentheses that might have made it this far
   }

   // if necessary, just choose part of the verse reference to pass to the web interfaces
   // they wouldn't know what to do with John 5:1-2, 5, 10-13 so I just give them John 5:1-2
   // this doesn't work quite right with something like 1:5,6 - it gets chopped to 1:5 instead of converted to 1:5-6
   if ($verse) {
       $verse = strtok($verse,',& ');
   }

   switch ($translation) {
        case 'ESV':
        // note: the ESV could actually support a mouseover reference
        // we could pull it directly from their site and include it in the $title text
        // http://www.gnpcb.org/esv/share/services/api/ for more info
             $link = 'http://www.gnpcb.org/esv/search/?go=Go&q=';
             $title = 'English Standard Version Bible';
             $link = sprintf('<a href="%s%s" title="%s">%s</a>',$link,htmlentities(urlencode(trim("$volume $book $verse"))),$title,trim($reference));
             break;
        case 'NET':
             $link = 'http://net.bible.org/passage.php?passage=';
             $title = 'New English Translation';
             $link = sprintf('<a href="%s%s" title="%s">%s</a>',$link,htmlentities(urlencode(trim("$volume $book $verse"))),$title,trim($reference));
             break;
	case 'TNIV':
             $link = 'http://www.tniv.info/bible/passagesearch.php?passage_request=';
             $title = 'Today\'s New International Version';
             $link = sprintf('<a href="%s%s" title="%s">%s</a>',$link,htmlentities(urlencode(trim("$volume $book $verse"))),$title,trim($reference));
	     break;
        default:
             $link = "http://biblegateway.com/cgi-bin/bible?language=english&version=$translation&passage=";
             $title = 'Bible Gateway';
             $link = sprintf('<a href="%s%s" title="%s">%s</a>',$link,htmlentities(urlencode(trim("$volume $book $verse"))),$title,trim($reference));
             break;
    }

    return $link;
}

?>
