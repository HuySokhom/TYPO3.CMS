<?php
/***************************************************************
*  Copyright notice
*
*  (c) 1999-2004 Kasper Skaarhoj (kasper@typo3.com)
*  All rights reserved
*
*  This script is part of the TYPO3 project. The TYPO3 project is
*  free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  The GNU General Public License can be found at
*  http://www.gnu.org/copyleft/gpl.html.
*  A copy is found in the textfile GPL.txt and important notices to the license
*  from the author is found in LICENSE.txt distributed with these scripts.
*
*
*  This script is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  This copyright notice MUST APPEAR in all copies of the script!
***************************************************************/
/**
 * Class for generation of the module menu.
 * Will make the the vertical, horizontal, selectorbox based menus AND the "about modules" display.
 * Basically it traverses the module structure and generates output based on that.
 *
 * $Id$
 * Revised for TYPO3 3.6 2/2003 by Kasper Skaarhoj
 * XHTML compliant content
 *
 * @author	Kasper Skaarhoj <kasper@typo3.com>
 */
/**
 * [CLASS/FUNCTION INDEX of SCRIPT]
 *
 *
 *
 *   72: class alt_menu_functions
 *   93:     function topMenu($theModules,$dontLink=0,$backPath='',$descr=0)
 *  386:     function mIconFile($Ifilename,$backPath)
 *  401:     function mIconFilename($Ifilename,$backPath)
 *  413:     function topButtons()
 *  425:     function adminButtons()
 *  444:     function adminFunctions($backPath)
 *  477:     function wrapLinkWithAB($link)
 *
 * TOTAL FUNCTIONS: 7
 * (This index is automatically created/updated by the extension "extdeveval")
 *
 */









/**
 * Class with menu functions
 *
 * @author	Kasper Skaarhoj <kasper@typo3.com>
 * @package TYPO3
 * @subpackage core
 */
class alt_menu_functions {

		// Internal
	var $fsMod = array();

	/**
	 * Creates the menu of modules.
	 *
	 * $descr determines the type of menu made:
	 * 		0 = Ordinary vertical menu
	 * 		1 = Descriptions for 'About modules' display
	 * 		2 = selector-box menu
	 * 		3 = topmenu - horizontal line of icons!
	 * 		4 = part of JavaScript switch contruct for alt_main.php frameset.
	 *
	 * @param	array		$theModules is the output from load_modules class ($this->loadModules->modules)
	 * @param	boolean		$dontLink == true will prevent the output from being linked with A-tags (used in the 'beuser' extension)
	 * @param	string		$backPath must be the 'backPath' to PATH_typo3 from where the the menu is displayed.
	 * @param	integer		$descr determines the type of menu made (see above)
	 * @return	string		The menu HTML
	 */
	function topMenu($theModules,$dontLink=0,$backPath='',$descr=0)	{
		global $LANG, $TBE_TEMPLATE, $BE_USER;

			// Initialize vars:
		$final='';
		$menuCode='';
		$descrCode='';
		$menuCode_sub='';
		$selectItems=array();
		$mIcons=array();
		$mJScmds=array();
		$onBlur=$GLOBALS['CLIENT']['FORMSTYLE'] ? 'this.blur();' : '';

		$selectItems[] = '<option value="">[ '.$LANG->sL('LLL:EXT:lang/locallang_core.php:buttons.selMenu_modules',1).' ]</option>';
		$mC=0;

			// Remove the 'doc' module?
		if ($BE_USER->getTSConfigVal('options.disableDocModuleInAB'))	{
			unset($theModules['doc']);
		}

			// Traverse array with modules
		reset($theModules);
		while(list($moduleName,$moduleInfo)=each($theModules))	{
			$mC++;

			$prefix='';
			if ($moduleInfo['navFrameScript'])	{
				if ($BE_USER->uc['condensedMode'])	{
					$prefix=$moduleInfo['navFrameScript'].'?currentSubScript=';
				} else {
					$prefix='alt_mod_frameset.php?'.
						'fW="+top.TS.navFrameWidth+"'.
						'&nav='.rawurlencode($moduleInfo['navFrameScript']).
						'&script=';
				}
				$this->fsMod[]='fsMod.recentIds["'.$moduleName.'"]="";';
			}

				// If there are submodules:
			if (is_array($moduleInfo['sub']))	{
					// Finding the default module to display
				if ($moduleInfo['defaultMod'])	{
					$link=$moduleInfo['sub'][$moduleInfo['defaultMod']]['script'];
				} else {
					reset($moduleInfo['sub']);
					$subTemp = current($moduleInfo['sub']);
					$link=$subTemp['script'];
				}
				$link_sub=1;	// Tells that the main modules links to a submodule
				$link='';	// Does not link to submodules...
			} else {
				$link=$moduleInfo['script'];
				$link_sub=0;
			}

			$moduleKey = $moduleName.'_tab';

				// Creating image icon
			$image = @getimagesize($this->mIconFile($LANG->moduleLabels['tabs_images'][$moduleKey],$backPath));
			$imageCode='';
			$descr3_title = $LANG->moduleLabels['tabs'][$moduleKey].' ';
			if ($image)	{
				$Ifilename = $this->mIconFilename($LANG->moduleLabels['tabs_images'][$moduleKey],$backPath);
				$imageCode = '<img src="'.$Ifilename.'" '.$image[3].' alt="" />';
				$descr3_imageCode = '<img src="'.$Ifilename.'" '.$image[3].' title="'.htmlspecialchars($descr3_title).'" alt="" />';
			} else {
				$descr3_imageCode = '<img'.t3lib_iconWorks::skinImg($backPath,'gfx/dummy_module.gif','width="14" height="12"').' title="'.htmlspecialchars($descr3_title).'" alt="" />';
			}

				// Creating the various links:
			$label = $LANG->moduleLabels['tabs'][$moduleKey];
			$JScmd='this.selectedIndex=0;';
			if ($link && $prefix)	$link=$prefix.rawurlencode($link);
			if ($link && !$dontLink)	{
				$JScmd='top.content.location=top.getModuleUrl(top.TS.PATH_typo3+\''.$this->wrapLinkWithAB($link).'\');';
				$label='<a href="#" onclick="top.goToModule(\''.$moduleName.'\');'.$onBlur.'return false;">'.$label.'</a>';	//  && !$link_sub
				$mIcons[]='<a href="#" onclick="top.goToModule(\''.$moduleName.'\');'.$onBlur.'return false;">'.$descr3_imageCode.'</a>';
				$mJScmds[]="case '".$moduleName."': \n ".$JScmd." \n break;";
			}

#			$selectItems[] = '<option value="'.htmlspecialchars($JScmd).'">'.htmlspecialchars($LANG->moduleLabels['tabs'][$moduleKey]).'</option>';
			$selectItems[] = '<option value="top.goToModule(\''.$moduleName.'\');">'.htmlspecialchars($LANG->moduleLabels['tabs'][$moduleKey]).'</option>';
			$label='&nbsp;<b>'.$label.'</b>&nbsp;';

				// Creating a main item for the vertical menu (descr=0)
			$menuCode.='
						<tr class="c-mainitem">
							<td colspan="3">'.$imageCode.'<span class="c-label">'.$label.'</span></td>
						</tr>';

				// Code for "About modules"
			$descrCode.='
						<tr class="c-mainitem bgColor4">
							<td colspan="3">'.$imageCode.$label.'</td>
						</tr>';




				// Travesing submodules
			$subCode='';
			if (is_array($moduleInfo['sub']))	{
				reset($moduleInfo['sub']);
				$c=0;
				while(list($subName,$subInfo)=each($moduleInfo['sub']))	{
					if ($c==0)		{
						$subCode.='
								<tr class="c-first">
									<td colspan="3"></td>
								</tr>';
						$descrCode.='
								<tr class="c-first">
									<td colspan="3"></td>
								</tr>';
					}

					$link = $subInfo['script'];
					$subKey = $moduleName.'_'.$subName.'_tab';

						// Creating image icon
					$image = @getimagesize($this->mIconFile($LANG->moduleLabels['tabs_images'][$subKey],$backPath));
					$imageCode='';
					$descr3_title = $LANG->moduleLabels['tabs'][$subKey].': '.$LANG->moduleLabels['labels'][$subKey.'label'];
					if ($image)	{
						$Ifilename = $this->mIconFilename($LANG->moduleLabels['tabs_images'][$subKey],$backPath);
						$imageCode = '<img src="'.$Ifilename.'" '.$image[3].' title="'.htmlspecialchars($LANG->moduleLabels['labels'][$subKey.'label']).'" alt="" />';
						$descr3_imageCode = '<img src="'.$Ifilename.'" '.$image[3].' title="'.htmlspecialchars($descr3_title).'" alt="" />';
					} else {
						$descr3_imageCode = '<img'.t3lib_iconWorks::skinImg($backPath,'gfx/dummy_module.gif','width="14" height="12"').' title="'.htmlspecialchars($descr3_title).'" alt="" />';
					}

						// Label for submodule:
					$label = $LANG->moduleLabels['tabs'][$subKey];
					$label_descr = ' title="'.htmlspecialchars($LANG->moduleLabels['labels'][$subKey.'label']).'"';
					$flabel = htmlspecialchars($label);
					$origLink = $link;
					if ($link && $prefix)	$link=$prefix.rawurlencode($link);

						// Setting additional JavaScript if frameset script:
					$addJS = '';
					if ($moduleInfo['navFrameScript'])	{$addJS="+'&id='+top.rawurlencode(top.fsMod.recentIds['".$moduleName."'])";}

						// If there is a script to link to (and linking is not disabled.
					if ($link && !$dontLink)	{
							// For condensed mode, send &cMR parameter to frameset script.
						if ($addJS && $BE_USER->uc['condensedMode'])	{$addJS.=  "+(cMR?'&cMR=1':'')";}

							// Command for the selector box:
						$JScmd = "
								top.content.location=top.getModuleUrl(top.TS.PATH_typo3+'".$this->wrapLinkWithAB($link)."'".$addJS.');
								top.fsMod.currentMainLoaded="'.$moduleName.'";';

							// If there is a frameset script in place:
						if (!$BE_USER->uc['condensedMode'] && $moduleInfo['navFrameScript'])	{
							$JScmd = '
								if (top.content.list_frame && top.fsMod.currentMainLoaded=="'.$moduleName.'") {
									top.currentSubScript="'.$origLink.'";
									top.content.list_frame.location=top.getModuleUrl(top.TS.PATH_typo3+"'.$this->wrapLinkWithAB($origLink).'"'.$addJS.');
								} else {
									top.content.location=top.TS.PATH_typo3+(
										top.nextLoadModuleUrl?
										"'.($prefix?$this->wrapLinkWithAB($link).'&exScript=':'').'listframe_loader.php":
										"'.$this->wrapLinkWithAB($link).'"'.$addJS.'
									);
									top.fsMod.currentMainLoaded="'.$moduleName.'";
								}';
						}
						$selectItems[] = '<option value="top.goToModule(\''.$moduleName.'_'.$subName.'\');">'.htmlspecialchars('- '.$label).'</option>';
						$onClickString = htmlspecialchars('top.goToModule(\''.$moduleName.'_'.$subName.'\');'.$onBlur.'return false;');
						$flabel='<a href="#" onclick="'.$onClickString.'"'.$label_descr.'>'.htmlspecialchars($label).'</a>';
						$mIcons[]='<a href="#" onclick="'.$onClickString.'"'.$label_descr.'>'.$descr3_imageCode.'</a>';
						$mJScmds[]="case '".$moduleName.'_'.$subName."': \n ".$JScmd." \n break;";
					}

					$subCode.='
							<tr class="c-subitem-row">
								<td></td>
								<td align="center">'.(!$BE_USER->uc['hideSubmoduleIcons']?$imageCode:'').'</td>
								<td class="c-subitem-label">'.$flabel.'</td>
							</tr>';

						// For "About modules":
					$descrCode.='
							<tr class="c-subitem-row">
								<td align="center">'.$imageCode.'</td>
								<td>'.$flabel.'&nbsp;&nbsp;</td>
								<td><strong>'.htmlspecialchars($LANG->moduleLabels['labels'][$subKey.'label']).'</strong><br />'.$LANG->moduleLabels['labels'][$subKey.'descr'].'</td>
							</tr>';

						// Possibly adding a divider line
					$c++;
					if ($c<count($moduleInfo['sub']))	{
							// Divider
						$subCode.='
							<tr class="c-divrow">
								<td colspan="3"><img'.t3lib_iconWorks::skinImg($backPath,'gfx/altmenuline.gif','width="105" height="3"').' alt="" /></td>
							</tr>';
					}
				}
					// Spacer gif for top menu:
				if (count($theModules)>$mC)	{
					$mIcons[]='<img src="'.$backPath.'gfx/acm_spacer2.gif" width="8" height="12" hspace="3" alt="" />';
				}
			}

				// Add spacers after each main section:
			$subCode.='
					<tr class="c-endrow">
						<td colspan="3"></td>
					</tr>';
			$descrCode.='
					<tr class="c-endrow">
						<td colspan="3"></td>
					</tr>';

				// Add sub-code:
			$menuCode.=$subCode;
		}

			// $descr==0:	Ordinary vertical menu
		if ($menuCode)	{
			$final = '


				<!--
					Vertical module menu, shown in left frame of backend.
				-->
				<table border="0" cellpadding="0" cellspacing="0" id="typo3-vmenu">
					'.$menuCode.'
				</table>';
		}

			// Output for the "About modules" module
		if ($descr==1)	{
			$descrCode = '


				<!--
					Listing of modules, for Help > About modules
				-->
				<table border="0" cellpadding="0" cellspacing="0" id="typo3-about-modules">
					'.$descrCode.'
				</table>';
			$final = $descrCode;
		}

			// selector-box menu
		if ($descr==2)	{

				// Add admin-functions for clearing caches:
			if ($BE_USER->isAdmin())	{
				$functionArray = $this->adminFunctions($backPath);
				if (count($functionArray))	{
					$selectItems[] = '<option value=""></option>';
					foreach($functionArray as $fAoptions)	{
						$selectItems[] = '<option value="'.htmlspecialchars("document.location='".$fAoptions['href']."';").'">[ '.htmlspecialchars($fAoptions['title']).' ]</option>';
					}
				}
			}

				// Logout item:
			$selectItems[] = '<option value=""></option>';
			$selectItems[] = '<option value="'.htmlspecialchars("top.location='logout.php';").'">[ '.$LANG->sL('LLL:EXT:lang/locallang_core.php:buttons.logout',1).' ]</option>';
			$final = '
				<select name="menuselector" onchange="eval(this.options[this.selectedIndex].value);">
					'.implode('
					',$selectItems).'
				</select>';
		}
			// topmenu - horizontal line of icons!
		if ($descr==3)	{
			$final = ''.implode('',$mIcons).'';
		}

			// Output for the goToModules() function in main frameset.
		if ($descr==4)	{
			$final = chr(10).implode(chr(10),$mJScmds).chr(10);
		}

			// Return result:
		return $final;
	}

	/**
	 * Returns $Ifilename readable for script in PATH_typo3.
	 * That means absolute names are just returned while relative names are prepended with $backPath (pointing back to typo3/ dir)
	 *
	 * @param	string		Icon filename
	 * @param	string		Back path
	 * @return	string		Result
	 * @see mIconFilename()
	 */
	function mIconFile($Ifilename,$backPath)	{
		if (t3lib_div::isAbsPath($Ifilename))	{
			return $Ifilename;
		}
		return $backPath.$Ifilename;
	}

	/**
	 * Returns relative filename to the $Ifilename (for use in img-tags)
	 *
	 * @param	string		Icon filename
	 * @param	string		Back path
	 * @return	string		Result
	 * @see mIconFile()
	 */
	function mIconFilename($Ifilename,$backPath)	{
		if (t3lib_div::isAbsPath($Ifilename))	{
			$Ifilename = '../'.substr($Ifilename,strlen(PATH_site));
		}
		return $backPath.$Ifilename;
	}

	/**
	 * Returns logout button.
	 *
	 * @return	string
	 */
	function topButtons()	{
		global $LANG;

		$out.= '<form action="logout.php" target="_top"><input type="submit" value="'.$LANG->sL('LLL:EXT:lang/locallang_core.php:buttons.logout',1).'" /></form>';
		return $out;
	}

	/**
	 * Returns logout button.
	 *
	 * @return	string
	 */
	function adminButtons()	{
		global $LANG;

		$functionArray = $this->adminFunctions('');

		$icons=array();
		foreach($functionArray as $fAoptions)	{
			$icons[]= '<a href="'.htmlspecialchars($fAoptions['href']).'">'.$fAoptions['icon'].'</a>';
		}

		return implode('',$icons);
	}

	/**
	 * Returns array with parts from which the admin functions can be constructed.
	 *
	 * @param	string		Backpath.
	 * @return	array
	 */
	function adminFunctions($backPath)	{
		global $LANG,$BE_USER,$TYPO3_CONF_VARS;

		$functions=array();

			// Clearing of cache-files in typo3conf/ + menu
		if ($TYPO3_CONF_VARS['EXT']['extCache'])	{
			$title = $LANG->sL('LLL:EXT:lang/locallang_core.php:rm.clearCache_allTypo3Conf');
			$functions[]=array(
				'title' => $title,
				'href' => $backPath.'tce_db.php?vC='.$BE_USER->veriCode().'&redirect='.rawurlencode(t3lib_div::getIndpEnv('TYPO3_REQUEST_SCRIPT')).'&cacheCmd=temp_CACHED',
				'icon' => '<img'.t3lib_iconWorks::skinImg($backPath,'gfx/clear_cache_files_in_typo3c.gif','width="21" height="18"').' title="'.htmlspecialchars($title).'" alt="" />'
			);
		}

			// Clear all page cache
		$title = $LANG->sL('LLL:EXT:lang/locallang_core.php:rm.clearCache_all');
		$functions[]=array(
			'title' => $title,
			'href' => $backPath.'tce_db.php?vC='.$BE_USER->veriCode().'&redirect='.rawurlencode(t3lib_div::getIndpEnv('TYPO3_REQUEST_SCRIPT')).'&cacheCmd=all',
			'icon' => '<img'.t3lib_iconWorks::skinImg($backPath,'gfx/clear_all_cache.gif','width="21" height="18"').' title="'.htmlspecialchars($title).'" alt="" />'
		);

			// Return functions
		return $functions;
	}

	/**
	 * Appends a '?' if there is none in the string already
	 *
	 * @param	string		Link URL
	 * @return	string
	 */
	function wrapLinkWithAB($link)	{
		if (!strstr($link,'?'))	{
			return $link.'?';
		} else return $link;
	}
}


if (defined('TYPO3_MODE') && $TYPO3_CONF_VARS[TYPO3_MODE]['XCLASS']['typo3/class.alt_menu_functions.inc'])	{
	include_once($TYPO3_CONF_VARS[TYPO3_MODE]['XCLASS']['typo3/class.alt_menu_functions.inc']);
}
?>
