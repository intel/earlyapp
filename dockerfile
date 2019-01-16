FROM clearlinux
RUN clrtrust generate
RUN swupd bundle-add software-defined-cockpit-dev

