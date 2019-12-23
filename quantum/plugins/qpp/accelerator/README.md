This is the Git subtree clone of the QPP project:
https://github.com/vsoftco/qpp

Notes for developers:

(1) Subtree set-up commands: 
(you don't need to do this if pulling from XACC because this has been already done) 

```
git remote add -f qpp https://github.com/vsoftco/qpp.git
git subtree add --prefix quantum/plugins/qpp/accelerator/src qpp master --squash 
```

(2) Pull/Sync with upstream: subtree pull

```
git fetch qpp master
git subtree pull --prefix quantum/plugins/qpp/accelerator/src qpp master --squash
```
