(window.webpackJsonp=window.webpackJsonp||[]).push([["1762"],{"0dXt":function(o,e,n){(window.__NEXT_P=window.__NEXT_P||[]).push(["/play",function(){var o=n("6B33");return{page:o.default||o}}])},"6B33":function(o,e,n){"use strict";n.r(e),n.d(e,"default",function(){return k});var t=n("0iUn"),a=n("sLSF"),d=n("MI3g"),u=n("a7VT"),l=n("Tit0"),c=n("q1tI"),i=n.n(c),r=n("nOHt"),w=n.n(r),s=n("b0oO"),f=n("iXVO"),p=function(){return window.oakwood&&oakwood.requestPlay()||console.info("mock call for requestPlay")},k=function(o){function e(o){var n;return Object(t.default)(this,e),(n=Object(d.default)(this,Object(u.default)(e).call(this,o))).state={},n}return Object(l.default)(e,o),Object(a.default)(e,[{key:"componentDidMount",value:function(){return window.oakwood?1==window.oakwood.data.isUpdating?(w.a.push("/download"),console.info("already updating")):(oakwood.checkForUpdates(),0==window.oakwood.data.alreadyInstalled?(w.a.push("/download"),void oakwood.requestUpdate()):void(!0!==window.oakwood.data.needsUpdate||w.a.push("/update"))):console.info("mock call for checkForUpdates")}},{key:"componentWillUnmount",value:function(){}},{key:"componentDidUpdate",value:function(){}},{key:"render",value:function(){return i.a.createElement(c.Fragment,null,i.a.createElement(s.a,null),i.a.createElement(f.b,null,"Ready to ",i.a.createElement(f.a,{onClick:p},"Play")))}}]),e}(c.Component)}},[["0dXt","5d41","9da1"]]]);