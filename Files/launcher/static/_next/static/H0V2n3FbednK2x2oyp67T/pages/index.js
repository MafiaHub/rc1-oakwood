(window.webpackJsonp=window.webpackJsonp||[]).push([["ce6e"],{Lev9:function(t,e,n){"use strict";var a=n("nOHt"),o=n.n(a);e.a=function(t,e){t&&t.res?(t.res.writeHead(303,{Location:e}),t.res.end()):o.a.replace(e)}},RNiq:function(t,e,n){"use strict";n.r(e),n.d(e,"default",function(){return w});var a=n("0iUn"),o=n("sLSF"),i=n("MI3g"),l=n("a7VT"),u=n("Tit0"),c=n("q1tI"),r=n.n(c),s=(n("nOHt"),n("b0oO")),d=n("iXVO"),f=n("Lev9"),w=function(t){function e(t){var n;return Object(a.default)(this,e),(n=Object(i.default)(this,Object(l.default)(e).call(this,t))).state={dots:"."},n}return Object(u.default)(e,t),Object(o.default)(e,[{key:"componentDidMount",value:function(){var t=this;try{window.external.invoke("init")}catch(e){}this.interval=setInterval(function(){var e="."==t.state.dots?"..":".."==t.state.dots?"...":(t.state.dots,".");t.setState({dots:e})},500),setTimeout(function(){try{var t=window.oakwood.data;t.loaded?(window.oakwood.checkForUpdates(),t.needsUpdate?Object(f.a)(null,"/update"):Object(f.a)(null,"/play")):Object(f.a)(null,"/setup")}catch(e){console.log("no context in browser mode"),setTimeout(function(){return Object(f.a)(null,"/download")},2e3)}},1e3)}},{key:"componentWillUnmount",value:function(){this.interval&&clearInterval(this.interval)}},{key:"render",value:function(){return r.a.createElement(c.Fragment,null,r.a.createElement(s.a,null),r.a.createElement(d.b,null,"Initializing ",this.state.dots))}}]),e}(c.Component)},vlRD:function(t,e,n){(window.__NEXT_P=window.__NEXT_P||[]).push(["/",function(){var t=n("RNiq");return{page:t.default||t}}])}},[["vlRD","5d41","9da1"]]]);