#!/bin/bash
python -c "v=open('version').read().split('.');open('version','w').write(('%s.%s.%d' % (v[0],v[1],int(v[2])+1)))"
rm wheelhouse/*
sudo chmod +666 /var/run/docker.sock
pipx run cibuildwheel --platform linux --archs x86_64
pipx run cibuildwheel --platform linux --archs aarch64
twine upload wheelhouse/*