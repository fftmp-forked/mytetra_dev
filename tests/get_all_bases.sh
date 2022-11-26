#!/usr/bin/env bash

set -euo pipefail

declare -A bases

bases['xintrea']='https://github.com/xintrea/mytetra_syncro/archive/refs/heads/master.zip'
bases['Rarrugas']='https://github.com/xintrea/mytetra_db_rarrugas/archive/refs/heads/master.zip'
bases['balas1']='https://github.com/balas1/mytetra/archive/refs/heads/master.zip' # named as 'Balas' on site
bases['YellowRaven']='https://github.com/YellowRaven/myTetra_repo/archive/refs/heads/master.zip'
bases['Yurons']='https://github.com/Yurons/mytetra/archive/refs/heads/master.zip'
bases['lesnik757']='https://github.com/lesnik757/mytetra/archive/refs/heads/master.zip'
bases['Shandor']='https://github.com/xintrea/mytetra_db_shandor/archive/refs/heads/master.zip'
bases['sirrichar']='https://github.com/sirrichar/sirdata/archive/refs/heads/master.zip'
bases['Anatolean']='https://bitbucket.org/xintrea/mytetra_anatolean/get/master.zip'
bases['armagedec']='https://github.com/armagedec/mytetra/archive/refs/heads/master.zip'
bases['SorokinRed']='https://github.com/SorokinRed/mytetra/archive/refs/heads/master.zip'
bases['deadelf79']='https://github.com/deadelf79/mytetra/archive/refs/heads/master.zip'
bases['Adgaver']='https://github.com/xintrea/mytetra_db_adgaver_new/archive/refs/heads/master.zip'
bases['Pipitos1983']='https://github.com/xintrea/mytetra_db_pipitos1983/archive/refs/heads/master.zip'
bases['silenn']='https://github.com/silenn/mind/archive/refs/heads/master.zip'
bases['shlyapnikova']='https://github.com/shlyapnikova/mytetra/archive/refs/heads/master.zip'
bases['Velonski']='https://github.com/Velonski/mytetra-database/archive/refs/heads/master.zip'
bases['and-semakin']='https://github.com/and-semakin/mytetra_data/archive/refs/heads/master.zip' # named as 'BrokeRU' on site
bases['Mcold']='https://github.com/xintrea/mytetra_db_mcold/archive/refs/heads/master.zip'
bases['Alensav']='https://github.com/xintrea/mytetra_db_alensav/archive/refs/heads/master.zip'
bases['Consp11']='https://github.com/xintrea/mytetra_db_consp11/archive/refs/heads/master.zip'
bases['Kozlov-AE']='https://github.com/xintrea/mytetra_db_kozlovae/archive/refs/heads/master.zip'
bases['Wwwlir']='https://github.com/xintrea/mytetra_db_wwwlir/archive/refs/heads/master.zip'
bases['DuwazSandbox']='https://github.com/DuwazSandbox/mytetra/archive/refs/heads/master.zip' # named as 'Duwaz' on site
bases['Tairesh']='https://github.com/Tairesh/mytetra/archive/refs/heads/master.zip'
bases['Ivnglkv']='https://bitbucket.org/xintrea/mytetra_db_ivnglkv/get/master.zip'
bases['kolyag87']='https://github.com/kolyag87/library/archive/refs/heads/master.zip'
bases['andyk101']='https://github.com/andyk101/mytetra/archive/refs/heads/master.zip'
bases['garik456456']='https://github.com/garik456456/MyTetra/archive/refs/heads/master.zip'
bases['SalexIzyh']='https://github.com/SalexIzyh/MyTetra/archive/refs/heads/master.zip'
bases['Bunny-Hop']='https://github.com/Bunny-Hop/mytetrabase/archive/refs/heads/master.zip' # named as 'RuDennn (Bunny-Hop)' on site
bases['manakaden']='https://github.com/manakaden/mytetra/archive/refs/heads/master.zip'
bases['vitvrn']='https://github.com/vitvrn/mytetra/archive/refs/heads/master.zip'
bases['Fanrok']='https://github.com/xintrea/mytetra_db_fanrok/archive/refs/heads/master.zip'
bases['Grimar']='https://github.com/Grimar/mytetra/archive/refs/heads/master.zip'
bases['Juryak']='https://github.com/Juryak/mytetra/archive/refs/heads/master.zip'
bases['Nicolasomsk']='https://github.com/xintrea/mytetra_db_nicolasomsk/archive/refs/heads/master.zip'
bases['Azatserikbaev']='https://github.com/xintrea/mytetra_db_azatserikbaev/archive/refs/heads/master.zip'
bases['Shut913']='https://github.com/xintrea/mytetra_db_shut913/archive/refs/heads/master.zip'
bases['RedSpecialL']='https://github.com/RedSpecialL/mytetra/archive/refs/heads/master.zip'

# absent (disappeared?) bases:
#bases['Soxsten']=''
#bases['Svoynickname']=''
#bases['Larson227']=''
#bases['Harpokrat']=''

out_dir="$1"
mkdir -p "${out_dir}"
cd "${out_dir}"


# assume cwd is correct
get_db() {
    db_name="$1"
    url="$2"
    echo "$db_name +++ $url"

    db_dir="./${db_name}/"
    mkdir "${db_dir}"
    cd "${db_dir}"

    wget --quiet "$url"
    unzip -q 'master.zip'
    rm 'master.zip'
    zip_content_dir=$(find . -mindepth 1 -maxdepth 1 -type d)
    find "${zip_content_dir}" -mindepth 1 -maxdepth 1 -exec mv {} './' \;
    rmdir "${zip_content_dir}"
}

export -f get_db

parallel --link -j "${#bases[@]}" get_db ::: "${!bases[@]}" ::: "${bases[@]}"
