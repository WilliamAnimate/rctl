# rctl

_cause systemctl is the only good thing in the whole systemd suite_.

---

## Wtf?

Yeah. This is a horribly written wrapper to the openrc command line tools, made for those comfortable with systemctl.

## What's wrong with it??

first, we have rc-update add **evremap**

then, we have rc-service **evremap** start

whereas on systemd...

systemctl enable **evremap**.service

systemctl start **evremap**.service

why are they in different places???

why do i need to run another command to do this????//

## How to?

Take this excerpt from the source code:

```c
/*
 * CLI draft
 *
 * rctl enable evremap .......... rc-update add evremap
 * rctl enable default evremap .. rc-update add evremap default
 * rctl enable boot evremap ..... rc-update add evremap boot
 * rctl enable RUNLEVEL evremap . rc-update add evremap RUNLEVEL
 *
 * rctl disable evremap ......... rc-update del evremap
 * rctl disable default evremap . rc-update del evremap default
 * rctl disable boot evremap .... rc-update del evremap boot
 * rctl disable RUNLEVEL evremap..rc-update del evremap RUNLEVEL
 * ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * also add --now versions of each to also run the assocated rc-service:
 * rctl disable evremap --now ... rc-update del evremap && rc-service evremap stop
 *
 * rctl start evremap ........... rc-service evremap start
 * rctl stop evremap ............ rc-service evremap stop
 */
```

## Why is the code so abysmal

heh

## I found a memory corruption bug

![skull_gone](https://cdn.discordapp.com/emojis/1098881040900173895.webp?animated=true)

if it's not a buffer overflow then i don't know what to do.

