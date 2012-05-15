/**
    testCmd.c - Unit tests for Cmd

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/*********************************** Locals ***********************************/

typedef struct TestCmd {
    MprCmd  *cmd;
    MprBuf  *buf;
    char    *program;
} TestCmd;

static void manageTestCmd(TestCmd *tc, int flags);

/************************************ Code ************************************/

static int initCmd(MprTestGroup *gp)
{
    TestCmd     *tc;

    gp->data = tc = mprAllocObj(TestCmd, manageTestCmd);
    tc->program = mprJoinPath(mprGetAppDir(gp), "runProgram" BIT_EXE);
    return 0;
}


static void manageTestCmd(TestCmd *tc, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(tc->cmd);
        mprMark(tc->buf);
        mprMark(tc->program);
    }
}


static void testCreateCmd(MprTestGroup *gp)
{
    TestCmd     *tc;
    
    tc = gp->data;
    tc->cmd = mprCreateCmd(gp->dispatcher);
    assert(tc->cmd != 0);
    mprDestroyCmd(tc->cmd);
    tc->cmd = 0;
    /* GC cleanup */
}


static void testRunCmd(MprTestGroup *gp)
{
    TestCmd     *tc;
    char        *result, command[MPR_MAX_PATH];
    int         status, exitStatus;

    tc = gp->data;
    tc->cmd = mprCreateCmd(gp->dispatcher);
    assert(tc->cmd != 0);

    /*
        runProgram reads from the input, so it requires stdin to be connected
     */
    mprSprintf(command, sizeof(command), "%s 0", tc->program);
    status = mprRunCmd(tc->cmd, command, NULL, &result, NULL, -1, MPR_CMD_IN);
    assert(result != NULL);
    assert(status == 0);

    exitStatus = mprGetCmdExitStatus(tc->cmd);
    assert(exitStatus == 0);

    mprDestroyCmd(tc->cmd);
    tc->cmd = 0;
}


static ssize withDataCallback(MprCmd *cmd, int channel, void *data)
{
    MprTestGroup    *gp;
    TestCmd         *tc;
    MprBuf          *buf;
    ssize           len, space;

    gp = data;
    tc = gp->data;
    assert(tc != NULL);
    buf = tc->buf;
    assert(buf != NULL);
    len = 0;
    
    switch (channel) {
    case MPR_CMD_STDIN:
        break;

    case MPR_CMD_STDERR:
        mprCloseCmdFd(cmd, channel);
        break;
    
    case MPR_CMD_STDOUT:
        space = mprGetBufSpace(buf);
        if (space < (MPR_BUFSIZE / 4)) {
            if (mprGrowBuf(buf, MPR_BUFSIZE) < 0) {
                mprAssert(0);
                mprCloseCmdFd(cmd, channel);
                return 0;
            }
            space = mprGetBufSpace(buf);
        }
        len = mprReadCmd(cmd, channel, mprGetBufEnd(buf), space);            
        if (len <= 0) {
            int status = mprGetOsError();
            mprLog(5, "Read %d (errno %d) from %s", len, status, (channel == MPR_CMD_STDOUT) ? "stdout" : "stderr");
            if (len == 0 || (len < 0 && !(status == EAGAIN || status == EWOULDBLOCK))) {
                mprCloseCmdFd(cmd, channel);
            } else {
                mprEnableCmdEvents(cmd, channel);
            }
            return len;
            
        } else {
            mprAdjustBufEnd(buf, len);
            mprAddNullToBuf(buf);
            mprEnableCmdEvents(cmd, channel);
        }
        break;

    default:
        /* Child death notification */
        break;
    }
    return len;
}


/*
    Match s2 as a prefix of s1
 */
static int match(char *s1, char *s2)
{
    int     rc;

    if (s1 == 0) {
        return 0;
    }
    rc = strncmp(s1, s2, strlen(s2));
    return rc;
}


static void testWithData(MprTestGroup *gp)
{
    MprBuf      *buf;
    TestCmd     *tc;
    char        *data, *env[16], *argv[16], line[80], *s, *tok;
    ssize       len, rc;
    int         argc, i, status, fd;

    assert(gp != 0);
    tc = gp->data;
    assert(tc != NULL);

    tc->cmd = mprCreateCmd(gp->dispatcher);
    assert(tc->cmd != 0);

    tc->buf = mprCreateBuf(MPR_BUFSIZE, -1);
    assert(tc->buf != 0);

    argc = 0;
    argv[argc++] = tc->program;
    argv[argc++] = "0";
    argv[argc++] = "a";
    argv[argc++] = "b";
    argv[argc++] = "c";
    argv[argc++] = NULL;

    i = 0;
    env[i++] = "CMD_ENV=xyz";
    env[i++] = "USERDOMAIN=localhost";
    env[i++] = NULL;
    mprSetCmdCallback(tc->cmd, withDataCallback, gp);
    
    // mprLog(0, "START");
    rc = mprStartCmd(tc->cmd, argc, (cchar**) argv, (cchar**) env, MPR_CMD_IN | MPR_CMD_OUT | MPR_CMD_ERR);
    assert(rc == 0);

    /*
        Write data to the child's stdin. We write so little data, this can't block.
     */
    fd = mprGetCmdFd(tc->cmd, MPR_CMD_STDIN);
    assert(fd > 0);

    for (i = 0; i < 10; i++) {
        mprSprintf(line, sizeof(line), "line %d\n", i);
        len = (int) strlen(line);
        rc = write(fd, line, (wsize) len);
        assert(rc == len);
        if (rc != len) {
            break;
        }
    }
    mprFinalizeCmd(tc->cmd);

    assert(mprWaitForCmd(tc->cmd, MPR_TEST_SLEEP) == 0);

    /*
        Now analyse returned data
     */
    buf = tc->buf;
    assert(mprGetBufLength(buf) > 0);
    if (mprGetBufLength(buf) > 0) {
        data = mprGetBufStart(buf);
        // print("GOT %s", data);
        s = stok(data, "\n\r", &tok);
        assert(s != 0);
        assert(match(s, "a b c") == 0);

        s = stok(0, "\n\r", &tok);
        assert(s != 0);
        assert(match(s, "CMD_ENV=xyz") == 0);

        for (i = 0; i < 10; i++) { 
            mprSprintf(line, sizeof(line), "line %d", i);
            s = stok(0, "\n\r", &tok);
            assert(s != 0);
            assert(match(s, line) == 0);
        }
        s = stok(0, "\n\r", &tok);
        assert(match(s, "END") == 0);
    }
    status = mprGetCmdExitStatus(tc->cmd);
    assert(status == 0);

    mprDestroyCmd(tc->cmd);
    tc->cmd = 0;
}


static void testExitCode(MprTestGroup *gp)
{
    TestCmd     *tc;
    char        *result, command[MPR_MAX_PATH];
    int         i, status;

    tc = gp->data;
    tc->cmd = mprCreateCmd(gp->dispatcher);
    assert(tc->cmd != 0);

    for (i = 0; i < 1; i++) {
        mprSprintf(command, sizeof(command), "%s %d", tc->program, i);
        status = mprRunCmd(tc->cmd, command, NULL, &result, NULL, -1, MPR_CMD_IN);
        assert(result != NULL);
        assert(status == i);
        if (status != i) {
            mprLog(0, "Status %d, result %s", status, result);
        }
    }
    mprDestroyCmd(tc->cmd);
    tc->cmd = 0;
}


/*
    Test invoking a command with no stdout/stderr capture.
 */
static void testNoCapture(MprTestGroup *gp)
{
    TestCmd     *tc;
    char        command[MPR_MAX_PATH];
    int         status;

    tc = gp->data;
    tc->cmd = mprCreateCmd(gp->dispatcher);
    assert(tc->cmd != 0);

    mprSprintf(command, sizeof(command), "%s 99", tc->program);
    status = mprRunCmd(tc->cmd, command, NULL, NULL, NULL, -1, MPR_CMD_IN);
    assert(status == 99);

    status = mprGetCmdExitStatus(tc->cmd);
    assert(status == 99);
    mprDestroyCmd(tc->cmd);
    tc->cmd = 0;
}


#define CMD_COUNT 8

static void testMultiple(MprTestGroup *gp)
{
    TestCmd     *tc;
    MprCmd      *cmds[CMD_COUNT];
    char        command[MPR_MAX_PATH];
    int         status, i;

    tc = gp->data;
    for (i = 0; i < CMD_COUNT; i++) {
        cmds[i] = mprCreateCmd(gp->dispatcher);
        assert(cmds[i] != 0);
        mprAddRoot(cmds[i]);
    }
    for (i = 0; i < CMD_COUNT; i++) {
        mprSprintf(command, sizeof(command), "%s 99", tc->program);
        status = mprRunCmd(cmds[i], command, NULL, NULL, NULL, -1, MPR_CMD_IN);
        assert(status == 99);
        status = mprGetCmdExitStatus(cmds[i]);
        assert(status == 99);
    }
    for (i = 0; i < CMD_COUNT; i++) {
        assert(cmds[i] != 0);
        mprDestroyCmd(cmds[i]);
        mprRemoveRoot(cmds[i]);
    }
}



MprTestDef testCmd = {
    "cmd", 0, initCmd, 0,
    {
        MPR_TEST(0, testCreateCmd),
        MPR_TEST(0, testRunCmd),
        MPR_TEST(0, testExitCode),
        MPR_TEST(0, testWithData),
        MPR_TEST(0, testNoCapture),
        MPR_TEST(0, testMultiple),
        MPR_TEST(0, 0),
    },
};

/*
    @copy   default
    
    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
    
    This software is distributed under commercial and open source licenses.
    You may use the GPL open source license described below or you may acquire 
    a commercial license from Embedthis Software. You agree to be fully bound 
    by the terms of either license. Consult the LICENSE.TXT distributed with 
    this software for full details.
    
    This software is open source; you can redistribute it and/or modify it 
    under the terms of the GNU General Public License as published by the 
    Free Software Foundation; either version 2 of the License, or (at your 
    option) any later version. See the GNU General Public License for more 
    details at: http://embedthis.com/downloads/gplLicense.html
    
    This program is distributed WITHOUT ANY WARRANTY; without even the 
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
    
    This GPL license does NOT permit incorporating this software into 
    proprietary programs. If you are unable to comply with the GPL, you must
    acquire a commercial license to use this software. Commercial licenses 
    for this software and support services are available from Embedthis 
    Software at http://embedthis.com 
    
    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
