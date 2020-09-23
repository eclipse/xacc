# Guide to contributing

Please read this if you intend to contribute to the project.

## Legal stuff

Apologies in advance for the extra work required here - this is necessary to comply with the Eclipse Foundation's
strict IP policy.

Please also read [this](http://wiki.eclipse.org/Development_Resources/Contributing_via_Git).

In order for any contributions to be accepted, you MUST do the following things:

* Sign the [Eclipse Contributor Agreement](http://www.eclipse.org/legal/ECA.php).
To sign the Eclipse Contributor Agreement you need to:

  * Obtain an Eclipse Foundation account. Anyone who currently uses Eclipse Bugzilla or Gerrit already has an Eclipse account.
If you don’t, you need to [register](https://dev.eclipse.org/site_login/createaccount.php).

  * Login to the [projects portal](https://projects.eclipse.org/), select “My Account”, and then the “Eclipse ECA” tab.

* Add your GitHub ID in your Eclipse Foundation account. Find this option in the "Personal Information" tab under "Edit Your Account".

* "Sign-off" your commits!

Every commit you make in your patch or pull request MUST be "signed off".

You do this by adding the `-s` flag when you make the commit(s), e.g.

    git commit -s -m "your commit message..."

## Making your changes

* Fork the repository on GitHub.
* Create a new branch for your changes.
* Make your changes.
* Make sure you include tests.
* Make sure the test suite passes after your changes.
* Commit your changes into that branch.
* Use descriptive and meaningful commit messages.
* If you have a lot of commits, squash them into a single commit.
* Make sure you use the `-s` flag when committing as explained above.
* Push your changes to your branch in your forked repository.

## Submitting the changes

Submit a pull request via the normal GitHub UI.
 
## After submitting

* Do not use your branch for any other development, otherwise further changes that you make will be visible in the PR.

# Credit

This document was originally written by the ICE team at

https://raw.githubusercontent.com/eclipse/ice/master/CONTRIBUTING.md

We have shamelessly copied, modified and co-opted it for our own repo and we
graciously acknowledge the work of the original authors.
