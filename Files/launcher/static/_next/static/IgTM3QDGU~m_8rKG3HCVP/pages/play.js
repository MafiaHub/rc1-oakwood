(window.webpackJsonp=window.webpackJsonp||[]).push([["1762"],{"0dXt":function(e,n,t){(window.__NEXT_P=window.__NEXT_P||[]).push(["/play",function(){var e=t("6B33");return{page:e.default||e}}])},"6B33":function(e,n,t){"use strict";t.r(n),t.d(n,"default",function(){return k});var o=t("0iUn"),a=t("sLSF"),u=t("MI3g"),c=t("a7VT"),d=t("Tit0"),l=t("q1tI"),i=t.n(l),r=t("nOHt"),f=t.n(r),w=t("b0oO"),s=t("iXVO"),p=function(){return window.oakwood&&oakwood.requestPlay()||console.info("mock call for requestPlay")},k=function(e){function n(e){var t;return Object(o.default)(this,n),(t=Object(u.default)(this,Object(c.default)(n).call(this,e))).state={},t}return Object(d.default)(n,e),Object(a.default)(n,[{key:"componentDidMount",value:function(){if(!window.oakwood)return console.info("mock call for checkForUpdates");window.oakwood.checkForUpdates(),!0===window.oakwood.data.needsUpdate&&f.a.push("/update")}},{key:"componentWillUnmount",value:function(){}},{key:"componentDidUpdate",value:function(){}},{key:"render",value:function(){return i.a.createElement(l.Fragment,null,i.a.createElement(w.a,null),i.a.createElement(s.b,null,"Ready to ",i.a.createElement(s.a,{onClick:p},"Play")))}}]),n}(l.Component)}},[["0dXt","5d41","9da1"]]]);