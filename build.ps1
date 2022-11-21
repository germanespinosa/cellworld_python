python -c "v=open('version').read().split('.');open('version','w').write(('%s.%s.%d' % (v[0],v[1],int(v[2])+1)))"
Remove-Item -LiteralPath "build" -Force -Recurse
Remove-Item wheelhouse/*
python -m pip wheel . --no-deps
pipx run cibuildwheel --platform linux --archs x86_64
pipx run cibuildwheel --platform linux --archs aarch64
pipx run cibuildwheel --platform windows --archs AMD64

python -m twine upload wheelhouse/*