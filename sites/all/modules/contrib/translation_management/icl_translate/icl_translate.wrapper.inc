<?php
/*
 * $Id: icl_translate.wrapper.inc,v 1.2 2010/08/26 13:11:16 jozik Exp $
 *
 * @file D6 to D7 migration wrapper.
 */

/**
 * Implements hook_permission().
 *
 * @return array
 */
function icl_translate_permission() {
  
  return array(
    ICL_TRANSLATE_MANAGE_JOBS => array(
      'title' => t('Can manage translation jobs'),
      'description' => ''
    ),
    ICL_TRANSLATE_DO_JOBS => array(
      'title' => t('Can do translation jobs'),
      'description' => ''
    ),
    ICL_TRANSLATE_VIEW_JOBS => array(
      'title' => t('Can view translation jobs'),
      'description' => ''
    )
  );
}