# See the file LICENSE for redistribution information.
#
# Copyright (c) 1996,2007 Oracle.  All rights reserved.
#
# $Id: test005.tcl,v 1.1.1.1 2009/04/20 07:15:18 jack Exp $
#
# TEST	test005
# TEST	Small keys/medium data
# TEST		Put/get per key
# TEST		Close, reopen
# TEST		Sequential (cursor) get/delete
# TEST
# TEST	Check that cursor operations work.  Create a database; close
# TEST	it and reopen it.  Then read through the database sequentially
# TEST	using cursors and delete each element.
proc test005 { method {nentries 10000} args } {
	eval {test004 $method $nentries "005" 0} $args
}