# 出力方法の設定
set dgrid 200, 200 qnorm 2
set pm3d map
set size square

# 出力形式の設定
set term pngcairo size 1080, 1080 font ",24"

# データファイルごとにプロットを行う
filelist = system("ls *.dat")
do for[filename in filelist] {
    set out filename.".png"
    unset key
    set zlabel "Times" rotate by 90
    # ファイル名からグラフタイトルに必要な情報を抜き出し
    # 何文字目という情報で切り出しているので1桁の数字しか対応していない
    # それ以上の桁数に対応させたい場合は0埋めでファイル名を出力した上で添字を変えよう
    type = filename[10:10]
    round = filename[17:17] + 0
    ishalf = (filename[18:21] eq "half") ? "+1/2" : ""
    set title type."-Type Syndrome, Round".round.ishalf

    splot filename using 1:2:3
}
