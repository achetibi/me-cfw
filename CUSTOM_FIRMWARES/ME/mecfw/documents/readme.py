#!/usr/bin/python
#!/coding=utf-8

import os
import sys
import codecs

#
what_is_this = '''CFW %s.%s%s Minimum Edition for 01g, 02g model by neur0n

-- What is this?--
This is CFW work on PSP1000 and PSP2000.

The degree of stability is still unknown. 
Please test it on your own and refer to different people's test reports.

From 3.71 M33 -> OK
From 4.01 M33 -> OK
From 5.00 M33 -> OK
From 5.50 GEN -> OK(may need to edit version.txt)
From 5.50 Prometheus -> OK(may need to edit version.txt)
From 6.20 TN-D -> OK
From 6.37 ME -> Error
From 6.38 ME -> OK
From 6.39 ME -> OK
From 6.20 PRO-B4 -> Error
From 6.35 PRO-B4 -> Error
From 6.20 PRO-B5 -> OK
From 6.35 PRO-B5 -> OK
From 6.39 PRO-B6 -> OK
'''

#
what_is_this_620 = '''CFW %s.%s%s Minimum Edition for 01g, 02g model by neur0n

-- What is this?--
This is CFW work on PSP1000 and PSP2000.

The degree of stability is still unknown. 
Please test it on your own and refer to different people's test reports.

From 3.71 M33 -> OK
From 4.01 M33 -> OK
From 5.00 M33 -> OK
From 5.50 GEN -> OK(may need to edit version.txt)
From 5.50 Prometheus -> OK(may need to edit version.txt)
From 6.20 TN-D -> OK
From 6.20 PRO-B4 -> Error
'''

#
what_is_this_jp = '''CFW %s.%s%s Minimum Edition for 01g, 02g model by neur0n

-- 何これ?--
PSP1000 と　PSP2000で動くCFWです。

From 3.71 M33 -> OK
From 4.01 M33 -> OK
From 5.00 M33 -> OK
From 5.50 GEN -> OK(version.txtを編集する必要があるかも)
From 5.50 Prometheus -> OK(version.txtを編集する必要があるかも)
From 6.20 TN-D -> OK
From 6.37 ME -> Error
From 6.38 ME -> OK
From 6.39 ME -> OK
From 6.20 PRO-B4 -> Error
From 6.35 PRO-B4 -> Error
From 6.20 PRO-B5 -> OK
From 6.35 PRO-B5 -> OK
From 6.39 PRO-B6 -> OK
'''

#
what_is_this_jp_620 = '''CFW %s.%s%s Minimum Edition for 01g, 02g model by neur0n

-- 何これ?--
PSP1000 と　PSP2000で動くCFWです。

From 3.71 M33 -> OK
From 4.01 M33 -> OK
From 5.00 M33 -> OK
From 5.50 GEN -> OK(version.txtを編集する必要があるかも)
From 5.50 Prometheus -> OK(version.txtを編集する必要があるかも)
From 6.20 TN-D -> OK
From 6.20 PRO-B4 -> Error
'''

#
what_is_this_lite = '''LCFW %s.%s%s LME installer for OFW %s.%s%s

-- What is this? -- 
This is LCFW for OFW %s.%s%s.

The degree of stability is still unknown. 
Please test it on your own and refer to different people's test reports.
'''

#
what_is_this_lite_jp = '''-- 何これ？--
FW%s.%s%s向けのLCFWです。

注意
このツールを使って何か不具合が出ても自己責任でお願いします。製作者は一切責任をとりません。
'''

#
how_to_install = '''
--How to Install--
First, you need to install CFW or HEN in your PSP.

1. copy UPDATE folder at ms0:/PSP/GAME/.
2. Put %s.%s%s official update at ms0:/PSP/GAME/UPDATE/%s.PBP, or let the program download it, you can use a wifi connection.
3. Run installer from xmb.

-- features --
Hold R trigger and turn on the psp, you can enter recovery menu.
Hold Home button and turn on the psp, you can Boot OFW.
'''

#
how_to_install_jp = '''
--インｽトール--
上のリストを参考に、予めCFW かHENをインストールしておくこと。

1. UPDATE フォルダを ms0:/PSP/GAME/にコピーする.
2. FW%s.%s%sの公式アップデータをリネームして ms0:/PSP/GAME/UPDATE/%s.PBP にコピーする.( Wifi環境がある場合にはPSPで直接DLできます。)
3. xmbから起動.

-- 機能--
R 押しながら起動するとrecovery menuに入れます.
Homeを押しながら起動するとOFWが起動します.
'''

#
how_to_install_lite = '''
-- How to use --
Copy "installer" folder and "launcher" folder at "ms0:/PSP/GAME/"

-- Preparation --
First. You need to install LCFW modules in your PSP.

1.Execute "LME Installer for %s.%s%s" from XMB.
2.You can select these action:
Press x to install LCFW.
Press [] to uninstall LCFW.
Press R to exit.

After the action, PSP will rboot.

-- Start LCFW --
1.Execute "LME Launcher for %s.%s%s" from XMB.
2. enjoy :)

-- How to enter RecoveryMenu? --
From XMB :Open VshMenu and select "Enter Recovery ->"
From LCFW:Execute Launcher again.
From OFW :Execute Launcher with hold "R".

-- How to mount UmdVideo? --
0. Enable UmdVideo option at RecoveryMenu->config->Use UmdVideo Patch (Go only).
1. copy Iso file at ms(ef)0:/ISO/VIDEO/ .
2. Open VshMenu and select Iso file name.
'''

#
how_to_install_lite_jp = '''
ｰｰファイルの説明

- installer (XMBでの表示名は "LME Installer for %s.%s%s")
flash0に必要なファイルを書き込むツールです。アンインストールもできます。

- launcher (XMBでの表示名は "LME Launcher for %s.%s%s" )
LCFWを起動するツールです。
上の installer で必要なモジュールを書き込んでいないと起動しません。


使い方。

1, OFW%s.%s%sでinstallerを起動してしばらく待つ。
2,
Press X to install modules
Press [] to uninstall modules
R to exit

と表示されるはずなので、インストールする場合は　Xを、
アンインストールする場合は　[]（四角)を押してください。
Rを押すと何もしないで終了します。

------ここから下はインストールを選んだ人向けの説明です。--------
3, PSPが再起動してXMBに戻るはずです。この状態ではまだOFWのままです。
4, XMBから launcher を起動します。
5, 自動的ににXMBに戻ります。これでLCFWが適用状態になっているはずです。

一度インストールしてしまえば、次回からはlauncherの起動のみでおｋです。

-- リカバリーモードに入るには？　--
XMBから　:VshMenuで　EnterRecovery-> を選択。
LCFWから：もう一度launcherを起動してください。
OFWから：Rを押しながらlauncherを起動してください。

-- UmdVideo のマウント --
0. RecoveryMenu->config->Use UmdVideo Patch の設定を有効にしてください(Go だけ).
1. ms(ef)0:/ISO/VIDEO/ にIsoをコピー.
2. VshMenu からマウントするIsoを選択.
'''

#
credit = '''
-- Credit --
Dark_AleX		: For his great CFW. This CFW is based on his 5.00 M33.
'''

#
credit_lite = '''
-- Credit --
some1/Davee/Proxima/Zecoxao: For their kxploit.
liquidzigong	: For his 639kxploit POC.
bbtgp			: For his PrxEncrypter.
Draan			: For his kirk-engine code.
J416			: For his rebootex sample.
N-zaki			: For providing PSPGo from him.
'''

#
credit_common = '''Moz				: For his beta test and translated language file in Italian.
BombingBasta	: For his translated language file in French.
Rapper_skull	: For his translated language file in Italian.
The Z			: For his translated language file in German.
largeroliker	: For his translated language file in Spanish.
bassiw			: For his translated language file in Netherlands.
Publikacii		: For his translated language file in Bulgarian.
Yoti			: For his translated language file in Russian.
ExzoTikFruiT	: For his translated language file in Russian.
ABCanG &Dadrfy &estuibal &plum &SnyFbSx &teck4 &wn : For their work to collect nid list.
Tipped OuT		: For his EBOOT.PBP ICON0.

Thanks to: josemetallica, Xeeynamo, theastamana, GovanifY, Reidenshi
for helping in translating Recovery and VshMenu.
'''

#
about_translating = '''
-- About translating the recovery menu --
To translate the recovery, create a text file in ms0:/seplugins/xx_recovery.txt or flash1:/xx_recovery.txt
To translate the VshMenu, create a text file in ms0:/seplugins/xx_vshmenu.txt or flash1:/xx_vshmenu.txt
"xx" is the language code of your language.

es -> spanish
en -> english
fr -> french
de -> german
it -> italian
ja -> japanese
ko -> korean
nl -> netherlands
pt -> portuguese
ru -> russian
ch1 -> chinese simplified
ch2 -> chinese traditional

To use custom font in RecoveryMenu and VshMenu, put font data at ms0:/seplugins/xx_ftable.bin or flash1:/xx_ftable.bin
"xx" is the language code of your language.
'''#

about_translating_jp = '''
-- RecoveryMenuのカスタムフォントとTxtについて--
ms0:/seplugins/xx_recovery.txt か flash1:/xx_recovery.txtに言語TXTファイルを置けばそれを読み込む仕様です。
"xx" に入るのが以下の言語コードです。

es -> spanish
en -> english
fr -> french
de -> german
it -> italian
ja -> japanese
ko -> korean
nl -> netherlands
pt -> portuguese
ru -> russian
ch1 -> chinese simplified
ch2 -> chinese traditional

VshMenuの場合のファイル名は　xx_vshmenu.txt です。
フォントのパスはms0:/seplugins/xx_ftable.binです。
'''

def usage():
	print ("Usage: " + sys.argv[0] + " [type] [devkit]\n")
	print ("   type  : You can use lite or full as type.")
	print ("   devkit: The firmware version to generate document.")

def main():
	len_arg = len(sys.argv)
	if(len_arg != 3):
		usage()
		sys.exit()

		
	cfw_type = sys.argv[1]
	version_str = sys.argv[2]

	if cfw_type == "full":
			if version_str == "620": 
				readme = what_is_this_620 %(version_str[0], version_str[1], version_str[2])+how_to_install %(version_str[0], version_str[1], version_str[2], version_str)+credit+credit_common+about_translating
				
				readme_jp = what_is_this_jp_620 %(version_str[0], version_str[1], version_str[2])+how_to_install_jp %(version_str[0], version_str[1], version_str[2], version_str)+credit+credit_common+about_translating_jp
				readme_jp.decode('utf-8')
			
			else:
				readme = what_is_this %(version_str[0], version_str[1], version_str[2])+how_to_install %(version_str[0], version_str[1], version_str[2], version_str)+credit+credit_common+about_translating
				
				readme_jp = what_is_this_jp %(version_str[0], version_str[1], version_str[2])+how_to_install_jp %(version_str[0], version_str[1], version_str[2], version_str)+credit+credit_common+about_translating_jp
				readme_jp.decode('utf-8')

			f = open(version_str+"_me_readme_en.txt", "wb")
			f.write(readme.encode('utf8'))
			f.close()
			
			f = open(version_str+"_me_readme_jp.txt", "wb")
			f.write(readme_jp)
			f.close()

	elif cfw_type == "lite":
			readme = what_is_this_lite %(version_str[0], version_str[1], version_str[2], version_str[0], version_str[1], version_str[2], version_str[0], version_str[1], version_str[2])+how_to_install_lite %(version_str[0], version_str[1], version_str[2], version_str[0], version_str[1], version_str[2])+credit_lite+credit_common+about_translating

			f = open(version_str+"_lme_readme_en.txt", "wb")
			f.write(readme)
			f.close()

			readme_jp = what_is_this_lite_jp %(version_str[0], version_str[1], version_str[2])+how_to_install_lite_jp %(version_str[0], version_str[1], version_str[2], version_str[0], version_str[1], version_str[2], version_str[0], version_str[1], version_str[2])+credit_lite+credit_common+about_translating_jp

			readme_jp.decode('utf-8')
			f = open(version_str+"_lme_readme_jp.txt", "wb")
			f.write(readme_jp)
			f.close()

	else:
		usage()
		sys.exit()


if __name__ == "__main__":
	main()
